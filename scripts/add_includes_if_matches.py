import glob
import re

files = (
    glob.glob("src/**/*.c", recursive=True)
    + glob.glob("src/**/*.cpp", recursive=True)
    + glob.glob("src/**/*.h", recursive=True)
    + glob.glob("src/**/*.hpp", recursive=True)
)


def main():
    for filename in files:
        with open(filename) as f:
            originals = s = f.read()

        matches = list(re.finditer('#include ".*\n', s))
        if not matches:
            matches = list(re.finditer("#include .*\n", s))

        if matches:
            splitpos = matches[0].start()
        else:
            matches = list(re.finditer("#pragma once\n", s))
            if not matches:
                matches = list(
                    re.finditer(
                        r"#ifndef\s+([A-Za-z0-9_]+)\s*\n#define\s+\1(\s[^\n]*)?\n", s
                    )
                )
            if matches:
                splitpos = matches[0].end()
            else:
                print("file %s has no includes" % filename)
                continue

        newincludes = []
        if re.search("[^A-Za-z0-9_]hello\(\)", s):
            if not re.search('#include "hello"', s):
                newincludes.append("hello")

        newincludes = sorted(newincludes)

        prefix = s[:splitpos]
        suffix = s[splitpos:]

        while True:
            m = re.match("#include <(.*)>", suffix)
            if m:
                prefix += suffix.split("\n", 1)[0] + "\n"
                suffix = suffix.split("\n", 1)[1]
            else:
                break

        for include in newincludes:
            while True:
                m = re.match('#include "(.*)"', suffix)
                if m and m.group(1).lower() < include.lower():
                    prefix += suffix.split("\n", 1)[0] + "\n"
                    suffix = suffix.split("\n", 1)[1]
                    continue
                else:
                    prefix += '#include "' + include + '"\n'
                    break

        s = prefix + suffix

        # s = prefix + "".join("#include <" + n + ">\n" for n in newincludes) + suffix

        if s != originals:
            print("changing " + filename)
            with open(filename, "w") as f:
                f.write(s)


main()
