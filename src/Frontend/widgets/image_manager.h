#pragma once

#include "default.h"

#include <cstdint>
#include <cstdio>
#include <vector>

#include "imgui/imgui.h"

#define MAX_IMAGE_WINDOW_NAME_LENGTH 0x20

struct ImageWindow {
    char name[MAX_IMAGE_WINDOW_NAME_LENGTH] = {};
    void* (*draw)();      // must return the value passed to imgui to draw
    bool enabled = true;
    size_t width;
    size_t height;
    size_t img_width;
    size_t img_height;
    void (*onclick)(bool left);
};

class ImageManager {

    std::vector<ImageWindow> windows = {};

public:
    ImageManager() {

    }

    void add_image_window(
            const char* name,
            void* (*draw)(),
            bool enabled,
            size_t width,
            size_t height,
            size_t img_width,
            size_t img_height,
            void (*onclick)(bool left)
            ) {
        ImageWindow window = {
                .draw = draw,
                .enabled = enabled,
                .width = width,
                .height = height,
                .img_width = img_width,
                .img_height = img_height,
                .onclick = onclick
        };
        STRCPY(window.name, MAX_IMAGE_WINDOW_NAME_LENGTH, name);
        windows.push_back(window);
    }

    void Draw(bool* p_open)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1),    ImVec2(-1, -1));
        ImGui::SetNextWindowSize(ImVec2(200, 310), ImGuiCond_Once);

        if (ImGui::Begin("Image Manager", p_open)) {
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetWindowContentRegionWidth(), 260), false);
            for (auto& window : windows) {
                ImGui::Selectable(window.name, &window.enabled);
            }
            ImGui::EndChild();
        }

        ImGui::End();

        for (auto& window : windows) {
            if (window.enabled) {
                void* texture = nullptr;
                if (window.draw) {
                    texture = window.draw();
                }

                ImGui::SetNextWindowSizeConstraints(ImVec2(window.width, window.height), ImVec2(FLT_MAX, FLT_MAX));
                ImGui::SetNextWindowSize(ImVec2(window.width, window.height), ImGuiCond_Once);
                ImGui::Begin(window.name, nullptr, ImGuiWindowFlags_HorizontalScrollbar);

                ImGui::Image(texture, ImVec2(window.img_width, window.img_height));

                ImGuiIO& io = ImGui::GetIO();
                ImVec2 pos = ImGui::GetCursorScreenPos();
                const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                const ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

                if (ImGui::IsItemHovered())
                {
                    if (window.onclick && ImGui::IsMouseClicked(0)) {
                        window.onclick(false);
                    }
                    else if (window.onclick && ImGui::IsMouseClicked(1)) {
                        window.onclick(true);
                    }

                    ImGui::BeginTooltip();
                    float region_sz = 32.0f;
                    float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                    float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                    region_y = window.img_height + region_y;
                    float zoom = 4.0f;
                    if (region_x < 0.0f) { region_x = 0.0f; }
                    else if (region_x > window.img_width - region_sz) { region_x = window.img_width - region_sz; }
                    if (region_y < 0.0f) { region_y = 0.0f; }
                    else if (region_y > window.img_height - region_sz) { region_y = window.img_height - region_sz; }
                    ImGui::Text("Min: (%.0f, %.0f)", region_x, region_y);
                    ImGui::Text("Max: (%.0f, %.0f)", region_x + region_sz, region_y + region_sz);
                    ImVec2 uv0 = ImVec2((region_x) / window.img_width, (region_y) / window.img_height);
                    ImVec2 uv1 = ImVec2((region_x + region_sz) / window.img_width, (region_y + region_sz) / window.img_height);
                    ImGui::Image(texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                    ImGui::EndTooltip();
                }

                ImGui::End();
            }
        }
    }
};