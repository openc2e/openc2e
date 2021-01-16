import os
import re
import sys

import clang.cindex
from clang.cindex import CursorKind


class replacement:
    def __init__(self, *, start, end, value):
        self.start = start
        self.end = end
        self.value = value

    def __eq__(self, other):
        return (
            self.start == other.start
            and self.end == other.end
            and self.value == other.value
        )

    def __ne__(self, other):
        return not (self == other)


class visitcontext:
    def __init__(self):
        self.original_filename = None
        self.indent = 0
        self.replacements = []

    def add_replacement(self, *, start, end, value):
        self.replacements.append(replacement(start=start, end=end, value=value))


def index_matching(seq, f):
    for i, x in enumerate(seq):
        if f(x):
            return i
    raise LookupError("Couldn't find anything")


def find_matching(seq, f):
    for x in seq:
        if f(x):
            return x
    raise LookupError("Couldn't find anything")


def any_member_refs(cursor, ctx):
    if cursor.kind == CursorKind.MEMBER_REF_EXPR:
        if len(list(cursor.get_children())) == 0 or (
            cursor.get_definition()
            and cursor.get_definition().semantic_parent.spelling == "caosVM"
        ):
            return True

    return any(any_member_refs(c, ctx) for c in cursor.get_children())


def fix_member_refs(cursor, ctx):
    if len(list(cursor.get_tokens())) == 0:
        # probably inside a macro, ignore but recurse into children
        # ugh we do this because python clang doesn't expose spellinglocation, just expansion location
        for c in cursor.get_children():
            fix_member_refs(c, ctx)
        return

    # print(
    #     "{} {} {}".format(cursor.kind, cursor.spelling, len(list(cursor.get_tokens())))
    # )
    if (
        cursor.kind == CursorKind.CALL_EXPR
        and re.search("^(c|s|v)_\w+", cursor.spelling)
        and list(cursor.get_tokens())[0].spelling == cursor.spelling
    ):
        print(cursor.kind, cursor.spelling)
        print([(t.kind, t.spelling) for t in cursor.get_tokens()])
        tokens = list(cursor.get_tokens())
        assert len(tokens) == 3
        ctx.add_replacement(
            start=tokens[1].extent.end.offset,
            end=tokens[1].extent.end.offset,
            value="vm",
        )
        return

    if cursor.kind == CursorKind.MEMBER_REF_EXPR and cursor.spelling != "vm":
        # definition = cursor.get_definition()
        # if cursor.spelling == "stop":
        # print(cursor.kind, cursor.spelling, definition.spelling if definition else None, definition.semantic_parent.spelling if definition else None, len(list(cursor.get_children())))
        if len(list(cursor.get_children())) == 0:
            # nice, add a "vm->" before it
            tokens = list(cursor.get_tokens())
            if len(tokens) != 1:
                # print(
                #     "WARNING: ignoring {}".format(" ".join(t.spelling for t in tokens))
                # )
                pass
            else:
                if str(tokens[0].extent.start.file) != ctx.original_filename:
                    # print("WARNING: wrong file {} {} {}".format(tokens[0].kind, tokens[0].spelling, tokens[0].extent))
                    pass
                if str(tokens[0].extent.start.file) == ctx.original_filename:
                    assert len(tokens) == 1
                    assert cursor.spelling == tokens[0].spelling
                    ctx.add_replacement(
                        start=tokens[0].extent.start.offset,
                        end=tokens[0].extent.start.offset,
                        value="vm->",
                    )
                # print(cursor.spelling)
                # for t in tokens:
                #     print("{} {}".format(t.spelling, t.extent))

    # if (
    #     cursor.kind == CursorKind.COMPOUND_STMT
    #     and len(list(cursor.get_tokens())) > 0
    #     and len(list(cursor.get_children())) == 0
    # ):
    #     print("WARNING: empty compound statement even though it has tokens")

    # print(list(cursor.canonical.get_children()))
    # print(list(cursor.canonical.get_arguments()))
    # print(cursor.canonical.get_definition())
    # print(list(cursor.get_tokens()))
    # clang.cindex.conf.lib.clang_visitChildren(
    #     cursor, clang.cindex.callbacks["cursor_visit"](myvisitor), None
    # )
    # print(list((c.kind, c.spelling) for c in cursor.walk_preorder()))
    for c in cursor.get_children():
        fix_member_refs(c, ctx)


def printall(cursor, ctx):
    print("{}{} {}".format("  " * ctx.indent, cursor.kind, cursor.spelling))
    ctx.indent += 1
    for c in cursor.get_children():
        printall(c, ctx)
    ctx.indent -= 1


def visit(cursor, ctx):
    if cursor.kind == CursorKind.TRANSLATION_UNIT:
        ctx.original_filename = cursor.spelling
    else:
        if str(cursor.location.file) != ctx.original_filename:
            # keep recursing so we get to the inside of macros
            # for c in cursor.get_children():
            #     visit(c, ctx)
            return
    #
    # print(
    #     "{}{} {} {} {}, {}".format(
    #         "  " * ctx.indent,
    #         cursor,
    #         cursor.kind,
    #         cursor.spelling,
    #         cursor.location.file,
    #         cursor.extent,
    #     )
    # )

    if cursor.kind == CursorKind.CXX_METHOD:
        full_name = "{}::{}".format(cursor.semantic_parent.spelling, cursor.spelling)

        if cursor.semantic_parent.spelling == "caosVM" and re.match(
            r"^(c|s|v)_(\w+)$", cursor.spelling
        ):
            print("cxx method {}".format(cursor.spelling))

            # find the method body and fixup member references
            body = find_matching(
                cursor.get_children(), lambda c: c.kind == CursorKind.COMPOUND_STMT
            )
            fix_member_refs(body, ctx)

            # rename it and add a caosVM* arg!
            try:
                # find the tokens that are caosVM and :: and remove them
                tokens = list(cursor.get_tokens())
                doublecolon_index = index_matching(tokens, lambda t: t.spelling == "::")
                doublecolon_token = tokens[doublecolon_index]
                classname_token = tokens[doublecolon_index - 1]
                assert classname_token.spelling == "caosVM"
                ctx.add_replacement(
                    start=classname_token.extent.start.offset,
                    end=doublecolon_token.extent.end.offset,
                    value="",
                )

                # find the params and add caosVM *vm
                assert len(list(cursor.get_arguments())) == 0
                name_token = tokens[doublecolon_index + 1]
                assert name_token.spelling == cursor.spelling
                lparen_token = tokens[doublecolon_index + 2]
                assert lparen_token.spelling == "("
                # are there any member references?
                needs_named_param = any_member_refs(body, ctx)
                ctx.add_replacement(
                    start=lparen_token.extent.end.offset,
                    end=lparen_token.extent.end.offset,
                    value="caosVM *vm" if needs_named_param else "caosVM*",
                )
            except LookupError:
                # whoops, probably a macro
                print("ignoring {}".format(cursor.spelling))
                pass

    else:
        # ctx.indent += 1
        # for c in cursor.get_children():
        #     visit(c, ctx)
        # ctx.indent -= 1
        pass


def unique(seq):
    result = []
    for x in seq:
        if x not in result:
            result.append(x)
    return result


def main():
    index = clang.cindex.Index.create()
    if len(sys.argv) == 1:
        sys.stderr.write("USAGE: {} [FILES...]\n".format(sys.argv[0]))
        exit(1)
    for filename in sys.argv[1:]:
        print(filename)
        ctx = visitcontext()
        ctx.original_filename = filename
        translationunit = index.parse(
            filename,
            args=[
                "-I",
                os.path.join(os.path.dirname(__file__), "src"),
                "-I",
                os.path.join(
                    os.path.dirname(__file__), "externals/mpark-variant/include"
                ),
                "-I",
                os.path.join(os.path.dirname(__file__), "externals/fmt/include"),
                "-I",
                os.path.join(
                    os.path.dirname(__file__), "externals/ghc_filesystem/include"
                ),
                "-isystem",
                "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1",
                "-isystem",
                "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.0/include",
                "-isystem",
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include",
                "-isystem",
                "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include",
                "-std=c++14",
            ],
            # options=clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD,
        )
        if translationunit.diagnostics:
            saw_error = False
            for d in translationunit.diagnostics:
                print(d)
                if d.severity in (
                    clang.cindex.Diagnostic.Error,
                    clang.cindex.Diagnostic.Fatal,
                ):
                    saw_error = True
            if saw_error:
                exit(1)
        for c in translationunit.cursor.get_children():
            visit(c, ctx)
        with open(filename, "rb") as f:
            data = bytearray(f.read())
        # unique it because macros can make a node show up multiple times
        for r in sorted(unique(ctx.replacements), key=lambda r: -r.end):
            # print("replace {} {} {}".format(r.start, r.end, repr(r.value)))
            data[r.start : r.end] = r.value.encode("utf-8")
        # fix comments
        data = re.sub(b"%pragma saveimpl caosVM::", b"%pragma saveimpl ", data)
        # whoops, I don't know why these happen
        # data = data.replace(b"vm->result.setvm->String", b"vm->result.setString")
        # data = data.replace(b"if (i vm->==", b"if (i ==")
        with open(filename, "wb") as f:
            f.write(data)


if __name__ == "__main__":
    main()
