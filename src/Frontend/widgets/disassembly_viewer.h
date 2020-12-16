#ifndef GC__DISASSEMBLY_VIEWER_H
#define GC__DISASSEMBLY_VIEWER_H

#include <cstdint>
#include <cinttypes>
#include <cstdlib>

#include "disasm.h"

#include "imgui/imgui.h"

#define INSTRS_BEFORE_PC 20
#define INSTRS_AFTER_PC 20


struct DisassemblyViewer
{
    uint32_t* PC;
    uint16_t (*read_instr)(uint32_t address);

    DisassemblyViewer()
    {

    }
    ~DisassemblyViewer()
    {

    }

    void Draw(bool* p_open)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1),    ImVec2(-1, -1));
        ImGui::SetNextWindowSize(ImVec2(400, (INSTRS_BEFORE_PC + INSTRS_AFTER_PC + 1) * 14), ImGuiCond_Once);

        if (!ImGui::Begin("Disassembly Viewer", p_open))
        {
            ImGui::End();
            return;
        }

        if (!read_instr || !this->PC) {
            // if nullptr is passed to memory, we can't disassemble anything
            // so just don't even start on the window then
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Disassembly Viewer"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::BeginChild("Disassembly");

        uint32_t address = *this->PC - 2;
        uint32_t current_PC = address;

        size_t count = INSTRS_BEFORE_PC + INSTRS_AFTER_PC + 1;

        address &= ~1;  // word align
        address -= (INSTRS_BEFORE_PC << 1);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < count; i++, address += 2) {
            uint16_t instr = read_instr(address);

            if (address == current_PC) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 100));
            }

            s_OpcodeInfo opcode = DisasmTable[instr];
            ImGui::Text("%08x:\t%04x\t%-10s", address, opcode.hex, opcode.mnemonic);

            if (address == current_PC) {
                ImGui::PopStyleColor(1);
            }
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
};

#endif //GC__DISASSEMBLY_VIEWER_H
