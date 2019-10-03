#pragma once

#pragma once
#include <vector>
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"


struct ChartData {
public:
	std::vector<ImVec2> data;
	ImU32 color;
};

class MyImGui {
public:
	static void DrawChart(const char* title, std::vector<ChartData*> data, ImVec2 position, ImVec2 size, ImVec2 min_range, ImVec2 max_range);
};
