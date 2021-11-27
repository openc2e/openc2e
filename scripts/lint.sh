#!/bin/sh

set -euo pipefail

if ! [ -x "$(command -v git)" ]; then
  echo >&2 "error: git is not installed"
  exit 1
fi

REQUIRED_CLANG_FORMAT_MAJOR=10
CLANG_FORMAT=clang-format
CLANG_FORMAT_MAJOR=clang-format-${REQUIRED_CLANG_FORMAT_MAJOR}

if [ -x "$(command -v $CLANG_FORMAT_MAJOR)" ]; then CLANG_FORMAT=$CLANG_FORMAT_MAJOR; fi

if ! [ -x "$(command -v $CLANG_FORMAT)" ]; then
  echo >&2 "error: clang-format is not installed"
  echo >&2 "Install clang-format version ${REQUIRED_CLANG_FORMAT_MAJOR}" 
  exit 1
fi

CLANG_FORMAT_VERSION=$($CLANG_FORMAT -version | cut -d' ' -f3)
CLANG_FORMAT_MAJOR=$(echo $CLANG_FORMAT_VERSION | cut -d'.' -f1)
if [ $CLANG_FORMAT_MAJOR != $REQUIRED_CLANG_FORMAT_MAJOR ]; then
  echo >&2 "warning: clang-format is wrong version (wanted $REQUIRED_CLANG_FORMAT_MAJOR, got $CLANG_FORMAT_MAJOR)"
fi

modified_files=$(git diff --name-only --diff-filter=ACMRTUXB HEAD)
for f in ${modified_files}; do
  if ! echo "${f}" | egrep -q "[.](cpp|c|mm|m|h)$"; then
    continue
  fi
  if ! echo "${f}" | egrep -q "^(src|tests)"; then
    continue
  fi
  f="$(git rev-parse --show-toplevel)/${f}"

  d=$($CLANG_FORMAT ${f} | (diff -u "${f}" - || true))
  if ! [ -z "${d}" ]; then
    echo "!!! ${f} not compliant to coding style, fixing:"
    echo "${d}"
    $CLANG_FORMAT -i "${f}"
  fi
done

