#!/usr/bin/env python
import difflib
import glob
import os
import shutil
import subprocess
import sys
import re

REQUIRED_CLANG_FORMAT_MAJOR = 10
COLOR_BOLD = "\033[1m" if sys.stdout.isatty() else ""
COLOR_GREEN = "\033[92m" if sys.stdout.isatty() else ""
COLOR_RED = "\033[91m" if sys.stdout.isatty() else ""
COLOR_BLUE = "\033[36m" if sys.stdout.isatty() else ""
COLOR_RESET = "\033[0m" if sys.stdout.isatty() else ""

def run(*args):
    p = subprocess.run(args, encoding="utf-8", capture_output=True, check=True)
    sys.stderr.write(p.stderr)
    return p.stdout

def eprint(*args):
    print(*args, file=sys.stderr)

if shutil.which(f"clang-format-{REQUIRED_CLANG_FORMAT_MAJOR}"):
    clang_format = f"clang-format-{REQUIRED_CLANG_FORMAT_MAJOR}"
elif shutil.which("clang-format"):
    clang_format = "clang-format"
    clang_format_version = run(clang_format, "--version").rstrip().split(" ")[2]
    clang_format_major = int(clang_format_version.split(".")[0])
    if clang_format_major != REQUIRED_CLANG_FORMAT_MAJOR:
        eprint(f"warning: clang-format is wrong version (wanted {REQUIRED_CLANG_FORMAT_MAJOR}, got {clang_format_version})")
else:
    eprint("error: clang-format is not installed.")
    eprint(f"Install clang-format version {REQUIRED_CLANG_FORMAT_MAJOR}")
    exit(1)

since_rev = sys.argv[1] if len(sys.argv) > 1 else "HEAD"
os.chdir(run("git", "rev-parse", "--show-toplevel").rstrip())
untracked_files = run("git", "ls-files", "-o", "--exclude-standard").split()
if since_rev == "0000000": # the null SHA means "lint everything"
    changed_files = glob.glob("**", recursive=True)
else:
    changed_files = run("git", "diff", "--name-only", "--diff-filter=ACMRTUXB", since_rev, "--").split()
for fname in sorted(untracked_files + changed_files):
    if not re.search(r"(?i)\.(cpp|c|mm|m|h)$", fname):
        continue
    if not re.search(r"^src[/\\]", fname):
        continue

    original = open(fname, "r").read()
    formatted = run(clang_format, fname)
    if formatted == original:
        continue

    print(f"{COLOR_BOLD}!!! {fname} not compliant to coding style, fixing:{COLOR_RESET}")
    open(fname, "w").write(formatted)

    diff = difflib.unified_diff(original.split("\n"), formatted.split("\n"), lineterm="")
    for line in diff:
        if line.startswith("---") or line.startswith("+++"):
            continue
        elif line.startswith("+"):
            print(f"{COLOR_GREEN}{line}{COLOR_RESET}")
        elif line.startswith("-"):
            print(f"{COLOR_RED}{line}{COLOR_RESET}")
        elif line.startswith("@@"):
            print(f"{COLOR_BLUE}{line}{COLOR_RESET}")
        else:
            print(line)
