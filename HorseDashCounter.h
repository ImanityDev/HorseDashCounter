#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "PersistentStorage.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

#include <chrono>


class HorseDashCounter: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	unsigned long horseDashes;
	unsigned long maxHorseDashes;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	bool flipChanged = false;
	bool hasFlip = true;
	void onLoad() override;
	void countHorseDashes();
	//void onUnload() override; // Uncomment and implement if you need a unload method

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void Render(CanvasWrapper canvas);
};
