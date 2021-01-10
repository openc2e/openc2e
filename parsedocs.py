import json
import os
import re
import sys

FILENAME_TO_CATEGORY = {  # default category mappings
    "caosVM_agent.cpp": "Agents",
    "caosVM_camera.cpp": "Cameras",
    "caosVM_cdplayer.cpp": "CD Player",
    "caosVM_core.cpp": "Core functions",
    "caosVM_creatures.cpp": "Creatures",
    "caosVM_compound.cpp": "Compound agents",
    "caosVM_debug.cpp": "Debugging",
    "caosVM_files.cpp": "Files",
    "caosVM_flow.cpp": "Flow control",
    "caosVM_genetics.cpp": "Genetics",
    "caosVM_history.cpp": "History",
    "caosVM_input.cpp": "Input",
    "caosVM_map.cpp": "Map",
    "caosVM_motion.cpp": "Motion",
    "caosVM_net.cpp": "Networking",
    "caosVM_ports.cpp": "Ports",
    "caosVM_resources.cpp": "Resources",
    "caosVM_scripts.cpp": "Scripts",
    "caosVM_sounds.cpp": "Sound",
    "caosVM_time.cpp": "Time",
    "caosVM_variables.cpp": "Variables",
    "caosVM_vectors.cpp": "Vectors",
    "caosVM_vehicles.cpp": "Vehicles",
    "caosVM_world.cpp": "World",
}


def parse_syntaxstring(syntaxstring):
    m = re.match(
        r"([A-Z0-9x#_:*!$+]+(\s+[A-Z0-9x#_:*!$+]+)?)\s+\(([^\)]+)\)", syntaxstring
    )
    name = m.groups()[0]
    type = m.groups()[2]
    arguments = []
    argline = syntaxstring[m.end() :].strip()
    while argline:
        m = re.match(r"(\w+)\s*\(([^\)]+)\)", argline)
        arguments.append({"name": m.groups()[0], "type": m.groups()[1]})
        argline = argline[m.end() :].strip()

    props = {
        "arguments": arguments,
        "name": name,
        "match": name.split(" ")[-1],
        "type": type,
        "syntaxstring": syntaxstring,
    }
    if " " in name:
        props["namespace"] = name.split(" ")[0].lower()

    return props


objects = []


def find_implementation(name, lines):
    p = 0
    while True:
        if lines[p].strip() == "" or lines[p].strip().startswith("//"):
            p += 1
            continue
        if lines[p].strip().startswith("/*"):
            p += 1
            while True:
                if lines[p].strip().endswith("*/"):
                    break
                p += 1
            p += 1
            continue
        break

    function_definition = re.search(
        "^\s*void ([cv]_[A-Za-z0-9_]+)\s*\(", lines[p]
    )
    if function_definition:
        return function_definition.group(1)

    caos_lvalue = re.search("^\s*CAOS_LVALUE(_[A-Z_]+)?\(([A-Za-z0-9_]+)\s*,", lines[p])
    if caos_lvalue:
        return "v_" + caos_lvalue.group(2)

    raise Exception(
        "Couldn't deduce function name for {} from {}".format(name, repr(lines[p]))
    )


for filename in sys.argv[1:]:
    with open(filename) as f:
        lines = f.read().split("\n")

    p = 0

    def getline():
        return re.sub(r"^\s*\*(\s+|$)", "", lines[p]).strip()

    try:
        while p < len(lines):
            if not getline().startswith("/**"):
                p += 1
                continue
            p += 1

            syntaxstring = getline()
            p += 1

            directives = []
            comments = []

            while True:
                if not getline().strip():
                    p += 1
                    continue
                if not re.match(r"^\s*%[^%]", getline()):
                    break
                directives.append(getline())
                p += 1

            while True:
                if getline().startswith("*/"):
                    p += 1
                    break
                comments.append(getline())
                p += 1

            def getdirective(prefix):
                result = None
                for d in directives:
                    m = re.match(r"%\s*{}\s+(.*)".format(prefix), d)
                    if not m:
                        continue
                    if result is not None:
                        raise Exception("%{} defined multiple times".format(prefix))
                    # print(m.groups())
                    result = m.groups()[0].strip()
                return result

            obj = {
                "filename": os.path.basename(filename),
                "category": FILENAME_TO_CATEGORY.get(
                    os.path.basename(filename), "unknown"
                ),
            }
            if "".join(comments).strip():
                obj["description"] = "\n".join(comments).strip() + "\n"
            obj.update(parse_syntaxstring(syntaxstring))

            obj["evalcost"] = {"default": 1 if obj["type"] == "command" else 0}
            cost = getdirective("cost")
            if cost:
                if " " in cost:
                    variants, cost = re.split(r"\s+", cost.strip())
                    for v in variants.split(","):
                        obj["evalcost"][v] = int(cost)
                else:
                    obj["evalcost"]["default"] = int(cost)

            obj["implementation"] = find_implementation(obj["name"], lines[p:])

            if getdirective("status"):
                obj["status"] = getdirective("status")

            if obj["type"] == "variable":
                obj["saveimpl"] = obj["implementation"].replace("v_", "s_")

            if getdirective("stackdelta"):
                obj["stackdelta"] = getdirective("stackdelta")
                if obj["stackdelta"] == "any":
                    obj["stackdelta"] = "INT_MAX"
                else:
                    obj["stackdelta"] = int(obj["stackdelta"])
            else:
                obj["stackdelta"] = 0 if obj["type"] == "command" else 1
                for a in obj["arguments"]:
                    if a["type"] != "variable":
                        obj["stackdelta"] -= 1

            if getdirective("variants"):
                variants = getdirective("variants")
                if variants == "all":
                    obj["variants"] = ("c1", "c2", "c3", "cv", "sm")
                else:
                    obj["variants"] = [_.strip() for _ in variants.strip().split(" ")]
            else:
                obj["variants"] = ("c3", "cv", "sm")

            for v in obj["variants"]:
                if v not in ("c1", "c2", "c3", "cv", "sm"):
                    raise Exception("Unknown variant '{}' when parsing command {}".format(v, obj['name']))

            for d in directives:
                d = d.replace("%", "").strip()
                if d.split(" ")[0] not in ("status", "cost", "stackdelta", "variants"):
                    raise Exception("Unknown directive: {}".format(d))

            obj["uniquename"] = (
                ("cmd_" if type == "command" else "expr_")
                + re.sub(r"(\s|[^A-Za-z0-9])+", "_", obj["name"])
                + "_" + "_".join(sorted(obj["variants"]))
            )

            objects.append(obj)
    except Exception as e:
        sys.stderr.write("{} at {} line {}\n".format(type(e).__name__, filename, p))
        raise

print(json.dumps(objects))
