#pragma once
#include "style.h"

class Framework : public Style
{
public:
	/* Helper Methods */
	void text_secondary(const char* text)
	{
		Style::styleTextSecondary();
		ImGui::Text(text);
		Style::style();
	}

	void default_style()
	{
		Style::style();
	}

	__inline void setY(int newY)
	{
		ImGui::SetCursorPosY(newY);
	}

	__inline void move(int newX, int newY)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + newX, ImGui::GetCursorPosY() + newY));
	}

	__inline void set(int newX, int newY)
	{
		ImGui::SetCursorPos(ImVec2(newX, newY));
	}

	__inline void CenterX_Undefined(int offset)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - offset);
	}

	__inline void CenterX_Text(const char* text)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - ImGui::CalcTextSize(text).x / 2);
		ImGui::Text(text);
	}

	__inline bool CenterX_Button(const char* text, int yOffset = 0)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize(text).x / 2) - (ImGui::GetStyle().FramePadding.x));
		ImGui::SetCursorPosY(yOffset);
		return ImGui::Button(text) == 1;
	}

	__inline void SecondaryText(const char* text)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, Style::FetchImVec4_RGB(42, 42, 42));
		ImGui::Text(text);
		ImGui::PopStyleColor();
	}

	__inline void SecondaryTextInput(int yOffset, const char* label, char* buf, const char* nullText, const char* icon, size_t len)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - 137.5f);
		ImGui::SetCursorPosY(yOffset);

		ImVec2 cp = ImGui::GetCursorPos();

		ImGui::PushStyleColor(ImGuiCol_Text, Style::FetchImVec4_RGB(42, 42, 42));
		
		ImGui::SetNextItemWidth(275);
		ImGui::InputText(label, buf, len);

		ImGui::SetCursorPos(ImVec2(cp.x + (ImGui::GetFontSize() * 0.85), cp.y + (ImGui::GetFontSize() / 3.f)));
		ImGui::Text(icon);

		if (strlen(buf) == 0)
		{
			ImGui::SetCursorPos( ImVec2( cp.x + (ImGui::GetFontSize() * 1.75), cp.y + (ImGui::GetFontSize() / 3.f)));
			ImGui::Text(nullText);
		}

		ImGui::PopStyleColor();
	}
};