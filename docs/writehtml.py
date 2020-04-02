import collections
import datetime
import re
import sys
import json


def captext(s):
    return s[0].upper() + s[1:]


def esc(t):
    return re.sub(r"\s+", "_", t)


with open(sys.argv[1]) as f:
    data = json.load(f)

# XXX
data = {"ops": data["variants"]["c3"]}

catsort = collections.defaultdict(lambda: [])
for key in sorted(data["ops"]):
    data["ops"][key]["key"] = key
    catsort[data["ops"][key]["category"]].append(data["ops"][key])

for key in catsort:
    # TODO: maybe?
    catsort[key] = sorted(catsort[key], key=lambda _: _["name"])

catl = [
    {"name": captext(_), "ents": catsort[_], "anchor": esc(_)} for _ in sorted(catsort)
]

time = datetime.datetime.now()
print(
    """<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/2002/REC-xhtml1-20020801/DTD/xhtml1-transitional.dtd">
<html>
\t<head>
\t\t<title>CAOS command reference - openc2e</title>
\t\t<link rel="stylesheet" type="text/css" href="docs.css" />
\t</head>
\t<body>
\t\t<h1>CAOS command reference - openc2e dev build</h1>"""
)

st_insert = {
    "todo": ["st_todo", "This command is not yet implemented.", "stubs", 0],
    "probablyok": [
        "st_maybe",
        "This command probably works, but it has not been annotated with its status.",
        "unknown",
        0,
    ],
    "maybe": [
        "st_maybe",
        "This command is believed to work, but has not yet been thoroughly tested.",
        "untested",
        0,
    ],
    "broken": [
        "st_todo",
        "This command is partially implemented but missing large amounts of functionality, or is nonconformant in some vital way.",
        "broken",
        0,
    ],
    "ok": ["st_ok", "This command works properly.", "done", 0],
}
st_insert["stub"] = st_insert["todo"]
st_insert["done"] = st_insert["ok"]

for op in data["ops"].values():
    st_insert[op["status"]][3] += 1

cstat = []
for clas in ("ok", "broken", "maybe", "probablyok", "todo"):
    if st_insert[clas][3] != 0:
        cstat.append("{} {}".format(st_insert[clas][3], st_insert[clas][2]))


sys.stdout.write('<div id="summary">')
sys.stdout.write("{} commands in total; ".format(len(data["ops"])))
sys.stdout.write(", ".join(cstat))
sys.stdout.write(".</div>")
sys.stdout.write('<div id="index"><ul>')
for c in catl:
    sys.stdout.write(
        '<li><a href="#c_' + c["anchor"] + '">' + c["name"] + "</a></li>\n"
    )
sys.stdout.write("</ul></div>\n")

print(
    """\t<div id="sidebar">
\t<h6>Commands</h6>
\t<ul>"""
)

for key in sorted(data["ops"]):
    if not key.startswith("c_"):
        continue
    classname = st_insert[data["ops"][key]["status"]][0] or "st_wtf"
    sys.stdout.write(
        '<li><a class="{}" href="#k_{}">{}</a></li>\n'.format(
            classname, key, data["ops"][key]["name"]
        )
    )

print(
    """\t</ul><hr />
\t<h6>Expressions</h6>
\t<ul>"""
)

for key in sorted(data["ops"]):
    if not key.startswith("v_"):
        continue
    classname = st_insert[data["ops"][key]["status"]][0] or "st_wtf"
    sys.stdout.write(
        '<li><a class="{}" href="#k_{}">{}</a></li>\n'.format(
            classname, key, data["ops"][key]["name"]
        )
    )

sys.stdout.write('</ul></div><div id="content">')
for cat in catl:
    print('<div class="category" id="c_{}">'.format(cat["anchor"]))
    print("<h2>{}</h2><hr/>".format(cat["name"]))
    for op in cat["ents"]:
        print('<div class="command" id="k_{}">'.format(op["key"]))
        print('<div class="header">')
        print('<span class="cmdname">{}</span>'.format(op["name"]))
        print('<span class="cmdtype">({})</span>'.format(op["type"]))
        for arg in op.get("arguments", []):
            print('<span class="argname">{}</span>'.format(arg["name"]))
            print('<span class="argtype">({})</span>'.format(arg["type"]))
        sys.stdout.write('</div><div class="description">')
        if op.get("description"):
            print('<div class="docs">{}</div>'.format(op["description"]))
        else:
            print('<div class="nodocs">This command is currently undocumented.</div>')
        sys.stdout.write('</div><div class="status">')
        if op["status"] in st_insert:
            sys.stdout.write('<div class="{}">'.format(st_insert[op["status"]][0]))
            sys.stdout.write(st_insert[op["status"]][1])
            sys.stdout.write("</div>")
        else:
            sys.stdout.write(
                '<div class="st_wtf">This command has an unknown status tag of {}.'.format(
                    op["status"]
                )
            )
            sys.stdout.write(
                "Please beat whoever set that status with a shovel or some other heavy object."
            )
            sys.stdout.write("</div>")
        sys.stdout.write("</div>")
        sys.stdout.write('<div class="administrivia"><ul>')

        sys.stdout.write("<li>Implemented in: {}</li>".format(op["filename"]))
        sys.stdout.write(
            "<li>Implementation functions (may be wrong): {}</li>".format(
                op["implementation"]
            )
        )
        if op.get("pragma"):
            sys.stdout.write("<li>Pragmas:<ul>")
            for pk in sorted(op["pragma"]):
                sys.stdout.write("<li>{} =&gt; {}</li>".format(pk, op["pragma"][pk]))
            sys.stdout.write("</ul></li>")
        sys.stdout.write("</ul></div>")
        sys.stdout.write("</div>")
    sys.stdout.write("</div>")

sys.stdout.write("</div></body></html>")
