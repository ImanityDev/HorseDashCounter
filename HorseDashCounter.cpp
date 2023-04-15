#include "pch.h"
#include "HorseDashCounter.h"


BAKKESMOD_PLUGIN(HorseDashCounter, "Count the number of horse dashes you did, yeehaw", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::shared_ptr<PersistentStorage> p_storage_;

void HorseDashCounter::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->registerCvar("horsedash_log", "0", "Set if logging is enabled");
	cvarManager->registerCvar("hdc_overlay_x", "1500", "X-Coordinate of the text overlay", true, true, 0, false, 0, true);
	cvarManager->registerCvar("hdc_overlay_y", "150", "Y-Coordinate of the text overlay", true, true, 0, false, 0, true);
	cvarManager->registerCvar("hdc_overlay_scale", "1", "Scaling of the text overlay", true, true, 0.1);

	p_storage_ = std::make_shared<PersistentStorage>(this, "", true, true);
	p_storage_->RegisterPersistentCvar("horsedash_record", "1", "All time horse dash record", true, true, 1, false, 0, true);
	p_storage_->Load();
	auto car = gameWrapper->GetLocalCar();
	if(car) hasFlip = car.HasFlip();
	horseDashes = 1;
	lastHorseDash = 1;
	maxHorseDashes = cvarManager->getCvar("horsedash_record").getIntValue();
	LOG(std::to_string(maxHorseDashes));
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});
	LOG("HorseDashCounter started!");
	cvarManager->registerNotifier("HorseDashCounter", [this](std::vector<std::string> args) {
		countHorseDashes();
		}, "", PERMISSION_ALL);

	begin = std::chrono::steady_clock::now();
	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick",
		[this](std::string eventName) {
			countHorseDashes();
		});
}

void HorseDashCounter::countHorseDashes() {
				if (!gameWrapper->IsInFreeplay()) { return; }
				ServerWrapper server = gameWrapper->GetCurrentGameState();
				if (!server) { return; }
				CarWrapper car = gameWrapper->GetLocalCar();
				if (!car) { return; }
				Vector velocity = car.GetVelocity();
				Vector angularVelocity = car.GetAngularVelocity();
				Vector location = car.GetLocation();
				if (car.HasFlip() != hasFlip) {
					if (horseDashes > maxHorseDashes) {
						maxHorseDashes = horseDashes;
						cvarManager->getCvar("horsedash_record").setValue((int)maxHorseDashes);
						p_storage_->WritePersistentStorage();
					}
					hasFlip = car.HasFlip();
					end = std::chrono::steady_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

					if (duration < 350) {
						if (!hasFlip) horseDashes++;
					}
					else {
						lastHorseDash = horseDashes;
						horseDashes = 1;
					}

					std::string output = std::format("HorseDashes: {}, Has Flip: {}, Time needed: {}ms", horseDashes, hasFlip, duration);
					auto log_cvar = cvarManager->getCvar("horsedash_log");
					if (log_cvar.getBoolValue()) LOG(output);
					begin = std::chrono::steady_clock::now();
				}
}

void HorseDashCounter::Render(CanvasWrapper canvas) {
	if (!gameWrapper->IsInFreeplay()) { return; }
	CVarWrapper xCvar = cvarManager->getCvar("hdc_overlay_x");
	CVarWrapper yCvar = cvarManager->getCvar("hdc_overlay_y");
	CVarWrapper sCvar = cvarManager->getCvar("hdc_overlay_scale");
	if (!xCvar || !yCvar || !sCvar) return;

	int x = xCvar.getIntValue();
	int y = yCvar.getIntValue();
	float s = sCvar.getFloatValue();

	std::string stats1 = std::format("Current Horse Dash counter: {}", horseDashes);
	std::string stats2 = std::format("Last Horse Dash: {}", lastHorseDash);
	std::string stats3 = std::format("Horse Dash Record: {}", maxHorseDashes);

	auto color = LinearColor{ 255.f, 100.f, 0.f, 255.f };
	canvas.SetColor(color);
	canvas.SetPosition(Vector2F{ (float) x, (float) y });
	canvas.DrawString(stats1, 1.5*s, 1.5*s, false);
	canvas.SetPosition(Vector2F{ (float) x, (float) y + 20*s });
	canvas.DrawString(stats2, 1.5*s, 1.5*s, false);
	canvas.SetPosition(Vector2F{ (float) x, (float) y + 40*s });
	canvas.DrawString(stats3, 1.5*s, 1.5*s, false);
}

void HorseDashCounter::RenderSettings() {
	ImGui::TextUnformatted("The best Horse Dash Counter you have ever seen, and probably the only one");
	CVarWrapper xCvar = cvarManager->getCvar("hdc_overlay_x");
	CVarWrapper yCvar = cvarManager->getCvar("hdc_overlay_y");
	CVarWrapper sCvar = cvarManager->getCvar("hdc_overlay_scale");
	if (!xCvar || !yCvar || !sCvar) return;

	int x = xCvar.getIntValue();
	if (ImGui::SliderInt("X-Coordinate", &x, 0, 4096)) {
		xCvar.setValue(x);
	}
	if (ImGui::IsItemHovered()) {
		std::string x_text = "X-Coordinate is " + std::to_string(x);
		ImGui::SetTooltip(x_text.c_str());
	}

	int y = yCvar.getIntValue();
	if (ImGui::SliderInt("Y-Coordinate", &y, 0, 2160)) {
		yCvar.setValue(y);
	}
	if (ImGui::IsItemHovered()) {
		std::string y_text = "Y-Coordinate is " + std::to_string(y);
		ImGui::SetTooltip(y_text.c_str());
	}

	float s = sCvar.getFloatValue();
	if (ImGui::SliderFloat("Scale", &s, 0.1, 10)) {
		sCvar.setValue(s);
	}
	if (ImGui::IsItemHovered()) {
		std::string s_text = "Scale is " + std::to_string(s);
		ImGui::SetTooltip(s_text.c_str());
	}

}

std::string HorseDashCounter::GetPluginName()
{
	return "Horse Dash Counter";
}

void HorseDashCounter::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}
