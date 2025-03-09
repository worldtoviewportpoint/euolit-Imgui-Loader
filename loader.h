#pragma once

#include <Windows.h>
#include <iostream>

#include "imgui.h"
#include "imspinner.hpp"

enum stage
{
	LOGIN,
	LOADING,
	DASHBOARD
};

#define WINDOW_WIDTH 850
#define WINDOW_HEIGHT 500

class Loader
{
public:
	stage currentStage = LOGIN;
	void Render();
	int ticks = 0;

};