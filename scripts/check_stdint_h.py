#!/usr/bin/env python

import glob
import os
import re

TOKEN_INCLUDES_CHECKS = [
    (
        ('uint8_t', 'int8_t', 'uint16_t', 'int16_t', 'uint32_t', 'int32_t', 'uint64_t', 'int64_t', 'uintptr_t'),
        ('stdint.h', 'cstdint')
    )
]

def has_tokens(text, tokens):
    m = re.search(r'[^\w](' + '|'.join(re.escape(t) for t in tokens) + r')[^\w]', text)
    return bool(m)

def has_includes(text, includes):
    m = re.search(r'[^\w](' + '|'.join(re.escape('#include <' + i + '>') for i in includes) + r')[^\w]', text)
    return bool(m)

def get_header_data(filename):
    stem, ext = os.path.splitext(filename)
    if not ext in ('.c', '.cpp'):
        return ""

    candidates = [
        stem + ".h",
        stem.removesuffix("Test") + '.h'
    ]
    for header_filename in candidates:
        if os.path.exists(header_filename):
            return remove_comments(open(header_filename).read())
    return ""

def remove_comments(text):
    return re.sub(r"/\*.*?\*/", "", re.sub(r"//[^\n]+", "", text))

for filename in sorted(glob.glob("src/**/*", recursive=True)):
    if os.path.splitext(filename)[1] not in ('.c', '.cpp', '.h', '.hpp'):
        continue
    if not os.path.isfile(filename):
            continue
    original_data = open(filename).read()
    data = remove_comments(original_data)
    header_data = get_header_data(filename)

    includes_to_add = []

    # find any includes that need to be added
    for (tokens, includes) in TOKEN_INCLUDES_CHECKS:
        if has_tokens(data, tokens) and not has_includes(data, includes) and not has_tokens(header_data, tokens):
            print(f"{filename} needs {includes[0]}")
            includes_to_add.append(includes[0])

    if not includes_to_add:
            continue
    
    # find place to add system includes
    system_include_match = re.search('#include <[^\n]+', original_data)
    if system_include_match:
         system_include_position = system_include_match.end()
    else:
        local_include_matches = list(re.finditer('#include "[^\n]+', original_data))
        if local_include_matches:
            system_include_position = local_include_matches[-1].end()
        else:
            raise Exception("Couldn't find any existing includes")

    newdata = original_data[:system_include_position] + "".join("\n#include <" + i + ">" for i in includes_to_add) + original_data[system_include_position:]
    with open(filename, 'w') as f:
         f.write(newdata)