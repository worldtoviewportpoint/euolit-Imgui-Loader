#pragma once
#include "imgui.h"

class Style
{
public:

	/* Instance Data */

	/* Helper Methods */

	void style()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.FrameRounding = 2.5f;
		style.FramePadding = ImVec2(45, 8);
		style.FrameBorderSize = 1.8f;

		style.Colors[ImGuiCol_WindowBg] = this->FetchImVec4_RGB(17, 17, 17);
		style.Colors[ImGuiCol_Text] = this->FetchImVec4_RGB(130, 130, 130);

		style.Colors[ImGuiCol_Button] = this->FetchImVec4_RGB(23, 23, 23);
		style.Colors[ImGuiCol_ButtonActive] = this->FetchImVec4_RGB(23, 23, 23);
		style.Colors[ImGuiCol_ButtonHovered] = this->FetchImVec4_RGB(23, 23, 23);

		style.Colors[ImGuiCol_FrameBg] = this->FetchImVec4_RGB(23, 23, 23);
		style.Colors[ImGuiCol_FrameBgActive] = this->FetchImVec4_RGB(23, 23, 23);
		style.Colors[ImGuiCol_FrameBgHovered] = this->FetchImVec4_RGB(35, 35, 35);

		style.Colors[ImGuiCol_Border] = this->FetchImVec4_RGB(40, 40, 40);
	}

	void styleTextSecondary()
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = this->FetchImVec4_RGB(54, 54, 54);
	}

	ImVec4 FetchImVec4_RGB(int r, int g, int b, int a = 255)
	{
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
};