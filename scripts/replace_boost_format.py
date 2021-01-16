import glob
import re

files = (
    glob.glob("src/**/*.c", recursive=True)
    + glob.glob("src/**/*.cpp", recursive=True)
    + glob.glob("src/**/*.re2c", recursive=True)
    + glob.glob("src/**/*.h", recursive=True)
    + glob.glob("src/**/*.hpp", recursive=True)
)


def read_string(s, p):
    basep = p

    assert s[p] == '"'
    p += 1

    while p < len(s):
        if s[p] == "\\":
            p += 2
        elif s[p] == '"':
            return s[basep + 1 : p]
        else:
            p += 1


def read_arg(s, p):
    basep = p

    in_string = False
    parens = 0
    square_parens = 0

    while p < len(s):
        # print('cursor:', s[p:p+20].rstrip())
        if in_string:
            if s[p] == "\\":
                p += 2
            elif s[p] == '"':
                in_string = False
                p += 1
            else:
                p += 1
            continue
        if square_parens > 0:
            if s[p] == "(":
                raise Exception
            elif s[p] == "]":
                square_parens -= 1
                p += 1
                continue
            else:
                p += 1
                continue
        if parens > 0:
            if s[p] == ")":
                # print("parens down")
                parens -= 1
                p += 1
                continue
            elif s[p] == '"':
                in_string = True
                p += 1
                continue
            elif s[p] == "(":
                parens += 1
                p += 1
                continue
            else:
                p += 1
                continue
        if s[p] in (" ", "\n", "\r", "\t", ",", ")", ";", "<"):
            # print("done")
            return s[basep:p]
        if s[p] == "(":
            # print("parens up")
            parens += 1
            p += 1
            continue
        if s[p] == "[":
            square_parens += 1
            p += 1
            continue
        if s[p] == '"':
            in_string = True
            p += 1
            continue
        p += 1
        continue


def advance_whitespace(s, p):
    basep = p
    while p < len(s) and s[p] in (" ", "\t", "\r", "\n"):
        p += 1
    return p - basep


def main():
    for filename in files:
        # print("Filename " + filename)
        with open(filename) as f:
            originals = s = f.read()

        while "boost::format(" in s:
            start = s.index("boost::format(")

            # assert s[start:].startswith("boost::format")

            p = start + len("boost::format(")
            # print("here", s[p:p+100])

            format_string = read_string(s, p)
            # print("format_string " + format_string)
            p += len(format_string) + 2  # quotes

            assert s[p] == ")"
            p += 1

            args = []

            while True:
                p += advance_whitespace(s, p)

                if s[p] in (")", ",", ";", "<"):
                    break

                # print("here2", s[p:p+10])
                assert s[p] == "%"
                p += 1

                p += advance_whitespace(s, p)

                arg = read_arg(s, p)
                # print('arg', arg)
                args.append(arg)

                p += len(arg)

                # print(s[p:p+100])
                # exit()

            # print("done")

            newpart = (
                'fmt::sprintf("'
                + format_string
                + '"'
                + "".join(", " + arg for arg in args)
                + ")"
            )

            s = s[:start] + newpart + s[p:]

            # print(s[p-200:p+100])
            # exit()

        while "str(format(" in s:
            start = s.index("str(format(") + len("str(")

            # assert s[start:].startswith("format")

            p = start + len("format(")
            # print("here", s[p:p+100])

            format_string = read_string(s, p)
            # print("format_string " + format_string)
            p += len(format_string) + 2  # quotes

            assert s[p] == ")"
            p += 1

            args = []

            while True:
                p += advance_whitespace(s, p)

                if s[p] in (")", ",", ";", "<"):
                    break

                # print("here2", s[p:p+10])
                assert s[p] == "%"
                p += 1

                p += advance_whitespace(s, p)

                arg = read_arg(s, p)
                # print('arg', arg)
                args.append(arg)

                p += len(arg)

                # print(s[p:p+100])
                # exit()

            # print("done")

            newpart = (
                'fmt::sprintf("'
                + format_string
                + '"'
                + "".join(", " + arg for arg in args)
                + ")"
            )

            s = s[:start] + newpart + s[p:]

            # print(s[p-200:p+100])
            # exit()

        while "boost::str(" in s:
            start = s.index("boost::str(")
            p = start + len("boost::str(")

            p += advance_whitespace(s, p)

            arg = read_arg(s, p)
            # print(repr(arg))
            p += len(arg)

            p += advance_whitespace(s, p)

            # print(repr(s[p:p+100]))

            assert s[p] == ")"

            s = s[:start] + s[start + len("boost::str(") : p] + s[p + 1 :]

            # exit()

        while "str(fmt::" in s:
            start = s.index("str(fmt::")
            p = start + len("str(")

            p += advance_whitespace(s, p)

            arg = read_arg(s, p)
            # print(repr(arg))
            p += len(arg)

            p += advance_whitespace(s, p)

            # print(repr(s[p:p+100]))

            assert s[p] == ")"

            s = s[:start] + s[start + len("str(") : p] + s[p + 1 :]

            # exit()

        s = s.replace("#include <boost/format.hpp>", "#include <fmt/printf.h>")
        s = s.replace("using boost::format;\n", "")
        s = s.replace("using boost::str;\n", "")

        if s != originals:
            print("changing " + filename)
            with open(filename, "w") as f:
                f.write(s)


main()
