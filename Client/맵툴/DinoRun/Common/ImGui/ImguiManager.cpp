#include "ImguiManager.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
}
ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}