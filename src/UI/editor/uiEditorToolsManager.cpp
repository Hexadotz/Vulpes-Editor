#include "uiEditorToolsManager.h"
#include "uiObjManager.h"
#include <imgui.h>
#include <iostream>

ImVec2 get_panel_size();
static int balls = 0;

ImVec2 get_panel_size() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	double x_size = viewport->Size.x - objectManagerUi::get_panel_size().x;

	return ImVec2(x_size, editorToolManagerUI::panel_y_size);
}

void editorToolManagerUI::draw_toolbar()
{
	ImVec2 position = ImVec2(objectManagerUi::get_panel_size().x, 19);
	ImVec2 size = get_panel_size();
	ImGui::SetNextWindowPos(position);
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("Tools", nullptr, panel_flags);

	/*if (ImGui::Button("select")) {
		current_tool_selected = TOOLS::SELECT_TOOL;
		balls = 0;
	}
	*/
	draw_tool_button("select", TOOLS::SELECT_TOOL);
	ImGui::SameLine();
	draw_tool_button("move", TOOLS::MOVE_TOOL);
	ImGui::SameLine();
	draw_tool_button("rotate", TOOLS::ROTATE_TOOL);
	ImGui::SameLine();
	draw_tool_button("scale", TOOLS::SCALE_TOOL);
	ImGui::SameLine();

	if (ImGui::Button("check"))
	{
		std::cout << balls ;
	}
	ImGui::End();
}

void editorToolManagerUI::draw_tool_button(const char* label, TOOLS tool_type)
{
	bool is_selected = (current_tool_selected == tool_type);
	if (editorToolManagerUI::current_tool_selected == tool_type) {
		ImGui::PushStyleColor(ImGuiCol_Button, color_selected);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color_hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color_active);
	}

	if (ImGui::Button(label))
	{
		current_tool_selected = tool_type;
	}
	if (is_selected)
	{
		ImGui::PopStyleColor(3);
	}
}

void editorToolManagerUI::draw_ui()
{
	editorToolManagerUI::draw_toolbar();
}
