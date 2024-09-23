//
//  imgui_color_gradient.cpp
//  imgui extension
//
//  Created by David Gallardo on 11/06/16.


#include "ImColorGradient.h"
#include "imgui_internal.h"

static const float GRADIENT_BAR_WIDGET_HEIGHT = 25;
static const float GRADIENT_BAR_EDITOR_HEIGHT = 40;
static const float GRADIENT_MARK_DELETE_DIFFY = 40;

namespace ImGui
{
    
    float4 m_cachedValues[256];

    void refreshCache(ColorGradient& gradient)
    {
        for (int i = 0; i < 256; ++i)
        {
            m_cachedValues[i] = gradient.CalculateColor(i / 255.0f);
        }
    }

    static void DrawGradientBar(ColorGradient& gradient,
        struct ImVec2 const& bar_pos,
        float maxWidth,
        float height)
    {
        ImVec4 colorA = { 1,1,1,1 };
        ImVec4 colorB = { 1,1,1,1 };
        float prevX = bar_pos.x;
        float barBottom = bar_pos.y + height;
        float prevMark = -1.0f;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        draw_list->AddRectFilled(ImVec2(bar_pos.x - 2, bar_pos.y - 2),
            ImVec2(bar_pos.x + maxWidth + 2, barBottom + 2),
            IM_COL32(100, 100, 100, 255));

        if (gradient.GetColorMarks().size() == 0)
        {
            draw_list->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
                ImVec2(bar_pos.x + maxWidth, barBottom),
                IM_COL32(255, 255, 255, 255));

        }

        ImU32 colorAU32 = 0;
        ImU32 colorBU32 = 0;
        const auto& colorMarks = gradient.GetColorMarks();
        auto endit = colorMarks.end();
        for (auto markIt = colorMarks.begin(); markIt != endit; ++markIt)
        {
            float mark = markIt->first;

            float from = prevX;
            float to = prevX = bar_pos.x + mark * maxWidth;

            if (prevMark == -1.0f)
            {
                colorA.x = gradient.GetColorMarks()[mark][0];
                colorA.y = gradient.GetColorMarks()[mark][1];
                colorA.z = gradient.GetColorMarks()[mark][2];
            }
            else
            {
                colorA.x = gradient.GetColorMarks()[prevMark][0];
                colorA.y = gradient.GetColorMarks()[prevMark][1];
                colorA.z = gradient.GetColorMarks()[prevMark][2];
            }

            colorB.x = gradient.GetColorMarks()[mark][0];
            colorB.y = gradient.GetColorMarks()[mark][1];
            colorB.z = gradient.GetColorMarks()[mark][2];

            colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
            colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

            if (mark > 0.0)
            {

                draw_list->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
                    ImVec2(to, barBottom),
                    colorAU32, colorBU32, colorBU32, colorAU32);
            }

            prevMark = mark;
        }

        if (prevMark && prevMark < 1.0)
        {

            draw_list->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
                ImVec2(bar_pos.x + maxWidth, barBottom),
                colorBU32, colorBU32, colorBU32, colorBU32);
        }

        ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
    }

    static void DrawGradientMarks(ColorGradient& gradient,
        float& draggingMark,
        float& selectedMark,
        struct ImVec2 const& bar_pos,
        float maxWidth,
        float height)
    {
        ImVec4 colorA = { 1,1,1,1 };
        ImVec4 colorB = { 1,1,1,1 };
        float barBottom = bar_pos.y + height;
        float prevMark = -1.0f;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImU32 colorAU32 = 0;
        ImU32 colorBU32 = 0;

        const auto& colorMarks = gradient.GetColorMarks();
        auto endit = colorMarks.end();
        for (auto markIt = colorMarks.begin(); markIt != endit; ++markIt)
        {
            float mark = markIt->first;

            if (!selectedMark)
            {
                selectedMark = mark;
            }

            float to = bar_pos.x + mark * maxWidth;

            if (prevMark == -1.0f)
            {
                colorA.x = gradient.GetColorMarks()[mark][0];
                colorA.y = gradient.GetColorMarks()[mark][1];
                colorA.z = gradient.GetColorMarks()[mark][2];
            }
            else
            {
                colorA.x = gradient.GetColorMarks()[prevMark][0];
                colorA.y = gradient.GetColorMarks()[prevMark][1];
                colorA.z = gradient.GetColorMarks()[prevMark][2];
            }

            colorB.x = gradient.GetColorMarks()[mark][0];
            colorB.y = gradient.GetColorMarks()[mark][1];
            colorB.z = gradient.GetColorMarks()[mark][2];

            colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
            colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

            draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 6)),
                ImVec2(to - 6, barBottom),
                ImVec2(to + 6, barBottom), IM_COL32(100, 100, 100, 255));

            draw_list->AddRectFilled(ImVec2(to - 6, barBottom),
                ImVec2(to + 6, bar_pos.y + (height + 12)),
                IM_COL32(100, 100, 100, 255), 1.0f, ImDrawFlags_None);

            draw_list->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1)),
                ImVec2(to + 5, bar_pos.y + (height + 11)),
                IM_COL32(0, 0, 0, 255), 1.0f, ImDrawFlags_None);

            if (selectedMark == mark)
            {
                draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 3)),
                    ImVec2(to - 4, barBottom + 1),
                    ImVec2(to + 4, barBottom + 1), IM_COL32(0, 255, 0, 255));

                draw_list->AddRect(ImVec2(to - 5, bar_pos.y + (height + 1)),
                    ImVec2(to + 5, bar_pos.y + (height + 11)),
                    IM_COL32(0, 255, 0, 255), 1.0f, ImDrawFlags_None);
            }

            draw_list->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3)),
                ImVec2(to + 3, bar_pos.y + (height + 9)),
                colorBU32, colorBU32, colorBU32, colorBU32);

            ImGui::SetCursorScreenPos(ImVec2(to - 6, barBottom));
            ImGui::InvisibleButton("mark", ImVec2(12, 12));

            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(0))
                {
                    selectedMark = mark;
                    draggingMark = mark;
                }
            }


            prevMark = mark;
        }

        ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
    }

    bool GradientButton(ColorGradient& gradient)
    {
        ImVec2 widget_pos = ImGui::GetCursorScreenPos();
        // ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float maxWidth = ImMax(250.0f, ImGui::GetContentRegionAvail().x - 100.0f);
        bool clicked = ImGui::InvisibleButton("gradient_bar", ImVec2(maxWidth, GRADIENT_BAR_WIDGET_HEIGHT));

        DrawGradientBar(gradient, widget_pos, maxWidth, GRADIENT_BAR_WIDGET_HEIGHT);

        return clicked;
    }

    bool GradientEditor(ColorGradient& gradient,
        float& draggingMark,
        float& selectedMark)
    {
        if (gradient.GetColorMarks().empty()) 
        {
            draggingMark = -1;
            selectedMark = -1;
        }
        
        bool modified = false;

        ImVec2 bar_pos = ImGui::GetCursorScreenPos();
        bar_pos.x += 10;
        float maxWidth = ImGui::GetContentRegionAvail().x - 20;
        float barBottom = bar_pos.y + GRADIENT_BAR_EDITOR_HEIGHT;

        ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            float pos = (ImGui::GetIO().MousePos.x - bar_pos.x) / maxWidth;

            float newMarkCol[4];
            float4 newMark = gradient.CalculateColor(pos);

            gradient.AddColorGradientMark(pos, newMark);
        }

        DrawGradientBar(gradient, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
        DrawGradientMarks(gradient, draggingMark, selectedMark, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);

        if (!ImGui::IsMouseDown(0) and draggingMark != -1.0f)
        {
            draggingMark = -1.0f;
        }

        if (ImGui::IsMouseDragging(0) and draggingMark != -1.0f)
        {
            float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
            bool insideZone = (ImGui::GetIO().MousePos.x > bar_pos.x) &&
                (ImGui::GetIO().MousePos.x < bar_pos.x + maxWidth);

            if (increment != 0.0f && insideZone)
            {
                float newDrag = draggingMark + increment;
                newDrag = ImClamp(newDrag, 0.0f, 1.0f);
                gradient.AddColorGradientMark(newDrag, gradient.GetColorMarks()[draggingMark]);
                gradient.RemoveColorGradientMark(draggingMark);
                draggingMark = newDrag;
                selectedMark = newDrag;
                refreshCache(gradient);
                modified = true;
            }

            float diffY = ImGui::GetIO().MousePos.y - barBottom;

            if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
            {
                gradient.RemoveColorGradientMark(draggingMark);
                draggingMark = -1.0f;
                selectedMark = -1.0f;
                modified = true;
            }
        }

        if (selectedMark == -1.0f && gradient.GetColorMarks().size() > 0)
        {
            selectedMark = gradient.GetColorMarks().begin()->first;
        }

        if (selectedMark != -1.0f)
        {
            bool colorModified = ImGui::ColorPicker4("Gradient Color", gradient.GetColor(selectedMark));

            if (selectedMark != -1.0f && colorModified)
            {
                modified = true;
                refreshCache(gradient);
            }
        }

        return modified;
    }
};
