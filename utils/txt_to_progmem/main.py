"""
Clarissa Verish
txt_to_progmem/main.py
2020-08-25

Converts a txt file containing a list of strings to the proper PROGMEM
file format for use in Arduino devices

"""

file = "dict.txt"
output = "dict.h"
# name of the variable to write to progmem
varName = "words"

lines = []

with open(file, "r") as f:
    lines = [line.strip() for line in f.readlines()]

l = str(len(lines))

with open(output, "w") as o:
    o.write("#include <pgmspace.h>\n")
    o.write("#define NUM_LINES\t\t" + l + "\n")
    o.write("const String " + varName + "[NUM_LINES] PROGMEM = {")
    for line in lines:
        o.write('"' + line + '", ')
    o.write("};")
