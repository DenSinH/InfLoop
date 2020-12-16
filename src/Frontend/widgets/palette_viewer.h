#pragma once

#include <cstdint>
#include <cstdio>

#include "imgui/imgui.h"

class PaletteViewer {

    const uint16_t* palette;

public:
    PaletteViewer(const uint8_t* palette) {
        this->palette = reinterpret_cast<const uint16_t*>(palette);
    }

    PaletteViewer() {
        this->palette = nullptr;
    }

    void Draw(bool* p_open)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1),    ImVec2(-1, -1));
        ImGui::SetNextWindowSize(ImVec2(330, 310), ImGuiCond_Once);

        if (!ImGui::Begin("Palette Viewer", p_open))
        {
            ImGui::End();
            return;
        }

        for (int i = 0; i < 16; i++) {

            // name
            ImGui::Text("4051%03x", i << 5); ImGui::SameLine();
            ImVec2 O = ImGui::GetCursorScreenPos();

            // values
            float sz = ImGui::GetTextLineHeight();
            for (int j = 0; j < 16; j++) {
                uint16_t color = 0x7fff;
                if (palette) {
                    color = palette[(i << 4) | j];
                }
                ImVec4 im_color = ImVec4(
                        (float)(color >> 10) / 31.0,
                        (float)((color >> 5) & 0x1f) / 31.0,
                        (float)(color& 0x1f) / 31.0,
                        1.0
                );
                ImGui::GetWindowDrawList()->AddRectFilled(
                        ImVec2(O.x + j * (sz + 3), O.y),
                        ImVec2(O.x + j * (sz + 3) + sz, O.y + sz),
                        ImColor(im_color)
                );
            }
            ImGui::NewLine();
        }

        ImGui::End();
    }
};