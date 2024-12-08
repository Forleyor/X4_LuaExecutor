#include <iostream>

#include "imgui.h"
#include "imgui_impl_win32.h"

#include "menu.h"
#include "../../utils/utils.h"
#include "../lua.h"

namespace ig = ImGui;

namespace Menu
{
	void InitializeContext(HWND hwnd) {
		if (ig::GetCurrentContext())
			return;

		RECT window;
		GetWindowRect(Utils::GetProcessWindow(), &window);
		
		gameWidth = static_cast<float>(window.right);
		gameHeight = static_cast<float>(window.bottom);
		menuWidth = gameWidth / 2;
		menuHeight = gameHeight / 2;

		ig::CreateContext();
		ImGui_ImplWin32_Init(hwnd);

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = io.LogFilename = nullptr;
		io.FontAllowUserScaling = true;

		editor = TextEditor();
		editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
		editor.SetTabSize(4);
		editor.SetText("");

		console = TextEditor();
		console.SetColorizerEnable(false);
		console.SetTabSize(4);
		console.SetReadOnly(true);
	}
	
	inline float SetHeightPercent(float value)
	{
		return menuHeight * value;
	}

    void Render() {
        if (!bShowMenu)
            return;
		
		ig::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_FirstUseEver);
		ig::Begin("Lua Executor");

		if (HandleWindowResize() == false)
		{
			ImGui::End();
			return;
		}

		editor.Render("LuaEditor", ImVec2(menuWidth, SetHeightPercent(0.45f)), true);
		if (autoExectute && editor.IsTextChanged() || ig::Button("Execute", ImVec2(250.0f, 50.0f)))
		{
			Lua::code = editor.GetText();
		}
		ig::SameLine();
		if (ig::Button("Clear Console", ImVec2(250.0f, 50.0f)))
		{
			console.SetText("");
		}
		ig::SameLine();
		ig::Checkbox("Auto Execute", &autoExectute);

		console.Render("Console", ImVec2(menuWidth, SetHeightPercent(0.45f)), true);

		ig::End();
    }

	bool HandleWindowResize()
	{
		ImVec2 view = ImGui::GetContentRegionAvail();

		if (view.x != menuWidth || view.y != menuHeight)
		{
			if (view.x == 0 || view.y == 0)
			{
				return false;
			}

			menuWidth = view.x;
			menuHeight = view.y;

			return true;
		}

		return true;
	}

}
