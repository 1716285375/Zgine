import os
import re

src_dir = 'f:/MyProject/Zgine/src'
include_dir = 'f:/MyProject/Zgine/include/Zgine'

# Mapping of basename -> full Zgine included path
header_to_zgine = {}

for root, dirs, files in os.walk(include_dir):
    for f in files:
        if f.endswith('.h'):
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, os.path.dirname(include_dir))
            rel_path = rel_path.replace('\\', '/')
            header_to_zgine[f] = f'<{rel_path}>'

def migrate_file(filepath):
    print(f"Migrating: {filepath}")
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 1. Update existing <Zgine/...> includes that might be outdated due to moves
    # We do this by finding all <Zgine/.*.h> and checking if the basename matches a new location
    def fix_zgine_include(match):
        full_inc = match.group(1)
        basename = os.path.basename(full_inc)
        if basename in header_to_zgine:
            return f'#include {header_to_zgine[basename]}'
        return match.group(0)

    content = re.sub(r'#include <(Zgine/.*?\.h)>', fix_zgine_include, content)

    # 2. Convert old #include "..." to <Zgine/...>
    def replace_include(match):
        inc_path = match.group(1)
        basename = os.path.basename(inc_path)
        if basename in header_to_zgine:
            return f'#include {header_to_zgine[basename]}'
        return match.group(0)

    content = re.sub(r'#include "(.*?\.h)"', replace_include, content)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

# Apply to all src
for root, dirs, files in os.walk(src_dir):
    for f in files:
        if f.endswith(('.cpp', '.h')):
            migrate_file(os.path.join(root, f))

# Apply to all headers in include/Zgine
for root, dirs, files in os.walk(include_dir):
    for f in files:
        if f.endswith('.h'):
            migrate_file(os.path.join(root, f))

# Apply to sandbox
migrate_file('f:/MyProject/Zgine/sandbox/main.cpp')
