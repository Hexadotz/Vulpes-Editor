#include "uiEditorToolsManager.h"

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

    draw_tool_button("Select", TOOLS::SELECT_TOOL);
    ImGui::SameLine();
    draw_tool_button("Move", TOOLS::MOVE_TOOL);
    ImGui::SameLine();
    draw_tool_button("Rotate", TOOLS::ROTATE_TOOL);
    ImGui::SameLine();
    draw_tool_button("Scale", TOOLS::SCALE_TOOL);
    ImGui::SameLine();

    ImGui::Checkbox("Snap", &uiManager::snap_to_grid);
    ImGui::SameLine();

    ImGui::Checkbox("Show Grid", &uiManager::draw_grid);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(75.0f);
    ImGui::InputInt("Grid Size", &uiManager::cell_size);
    ImGui::SameLine();

    if (ImGui::Button("Center Cam"))
    {
        Engine::Application::center_camera();
    }
    ImGui::SameLine();

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
