from bs4 import BeautifulSoup
import re

with open("./Renesas SH Instruction Set Summary.html", "r") as f:
    html = BeautifulSoup(f.read(), features="lxml")

opcodes = {}

for cont in html.body.findAll("div", attrs={"class": "col_cont"}):
    if "SH1" not in cont.text:
        continue

    asm = cont.findChild("div", attrs={"class": "col_cont_2"}).text
    bits = cont.findChild("div", attrs={"class": "col_cont_4"}).text
    issue_cycles = int(re.split(r"\D", cont.findChild("div", attrs={"class": "col_cont_7"}).text)[0])
    latency_cycles = int(re.split(r"\D", cont.findChild("div", attrs={"class": "col_cont_8"}).text)[0])

    _opcodes = {
        bits: asm
    }

    print(bits)
    for _bits, _asm in list(_opcodes.items()):
        if "Rn" in _asm and "nnnn" in _bits:
            _opcodes.pop(_bits)
            for n in range(16):
                _opcodes[re.sub("nnnn", "{0:04b}".format(n), _bits)] = _asm.replace("Rn", f"R{n}")

    for _bits, _asm in list(_opcodes.items()):
        if "Rm" in _asm and "mmmm" in _bits:
            _opcodes.pop(_bits)
            for m in range(16):
                _opcodes[re.sub("mmmm", "{0:04b}".format(m), _bits)] = _asm.replace("Rm", f"R{m}")

    for _bits, _asm in list(_opcodes.items()):
        if "imm" in _asm and "iiiiiiii" in _bits:
            _opcodes.pop(_bits)
            for imm in range(256):
                _opcodes[re.sub("iiiiiiii", "{0:08b}".format(imm), _bits)] = _asm.replace("imm", f"{imm}")

    for _bits, _asm in list(_opcodes.items()):
        if "disp" in _asm and "dddddddd" in _bits:
            _opcodes.pop(_bits)
            for disp in range(256):
                _opcodes[re.sub("dddddddd", "{0:08b}".format(disp), _bits)] = _asm.replace("disp", f"{disp}")

    # disp is 4 bits sometimes
    for _bits, _asm in list(_opcodes.items()):
        if "disp" in _asm and "dddd" in _bits:
            _opcodes.pop(_bits)
            for disp in range(16):
                _opcodes[re.sub("dddd", "{0:04b}".format(disp), _bits)] = _asm.replace("disp", f"{disp}")

    # bra/bf/bt/bsr opcodes
    for _bits, _asm in list(_opcodes.items()):
        if "label" in _asm and "dddddddddddd" in _bits:
            _opcodes.pop(_bits)
            for disp in range(2 ** 12):
                _opcodes[re.sub("dddddddddddd", "{0:012b}".format(disp), _bits)] = _asm.replace("label", f"label: +2 * {disp}")
    for _bits, _asm in list(_opcodes.items()):
        if "label" in _asm and "dddddddd" in _bits:
            _opcodes.pop(_bits)
            for disp in range(2 ** 12):
                _opcodes[re.sub("dddddddd", "{0:08b}".format(disp), _bits)] = _asm.replace("label", f"label: +2 * {disp}")

    # for _bits, _asm in _opcodes.items():
    #     print(f"    {_bits}: {_asm}")
    opcodes.update({b: (a, issue_cycles, latency_cycles) for b, a in _opcodes.items()})

print(len(opcodes))
for opcode in opcodes.keys():
    if not re.match("[01]{16}", opcode):
        print(opcode, opcodes[opcode])

with open("./disasm.h", "w+") as f:
    f.write("#ifndef INFLOOP_DISASM_H\n#define INFLOOP_DISASM_H")
    f.write("\n\n#include \"default.h\"")
    f.write("\n#include <map>")
    f.write("\n\n")
    f.write("""
typedef struct s_OpcodeInfo {
    u16 hex;
    const char* asm;
    int issue_cycles;
    int latency_cycles;
} s_OpcodeInfo;

const static std::map<u16, s_OpcodeInfo> DisasmTable = {""")

    for bits, (asm, issue_cycles, latency_cycles) in opcodes.items():
        f.write(f"\n    {{ 0b{bits}, {{ {hex(int(bits, 2))}, \"{asm}\", {issue_cycles}, {latency_cycles} }} }},")
    f.write("\n};")

    f.write("\n\n#endif")