#include "loader.h"

void Loader::Render()
{
	if (this->currentStage == LOADING)
	{
		ImVec2 windowCenter = ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

		// Loading Thing
		ImGui::SetNextWindowPos(ImVec2(windowCenter.x - 75, windowCenter.y - 75));
		ImGui::SetNextWindowSize(ImVec2(150, 150));
		ImGui::Begin(
			"##loader_spin",
			nullptr,
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration
		);

		ImGui::GetForegroundDrawList()->AddRectFilled(
			ImVec2(windowCenter.x - 100, windowCenter.y - 100),
			ImVec2(windowCenter.x + 75, windowCenter.y + 75),
			IM_COL32(0, 0, 0, 200),
			15.f
		);

		//ImSpinner::SpinnerAngEclipse("##Loader", 50.f, 1.f);
		ImSpinner::SpinnerGalaxyDots("##Loader", 50.f, 2.5f, ImColor(255, 255, 255, 255), 0.5f, 3);
		ImGui::End();

		this->ticks++;

		if (this->ticks > 500)
		{
			std::cout << "Change: " << this->ticks << std::endl;
			this->currentStage = DASHBOARD;
		}

	}
}

/*
#ifdef DEBUG
		std::cout << "[Begin Render]" << std::endl;
#endif
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(0, 0),
			ImVec2(ImGui::GetWindowSize()),
			IM_COL32(0, 0, 0, 175),
			0.f
		);
#ifdef DEBUG
		std::cout << "[DrawList Update]" << std::endl;
#endif

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 50, ImGui::GetWindowSize().y / 2 - 50));

		//ImSpinner::SpinnerAngEclipse("##Loader", 50.f, 1.f);

#ifdef DEBUG
		std::cout << "[Ended Render]" << std::endl;
#endif

*/