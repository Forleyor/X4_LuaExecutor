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
			//ImGui::PopStyleVar();
			return;
		}

		editor.Render("LuaEditor", ImVec2(menuWidth, SetHeightPercent(0.45f)), true);
		if (autoExectute && editor.IsTextChanged() || ig::Button("Execute", ImVec2(SetHeightPercent(0.50f), SetHeightPercent(0.10f))) && x4_LuaState)
		{
			if (_luaL_loadstring(x4_LuaState, editor.GetText().c_str()) == LUA_OK)
			{
				if (autoExectute)
				{
					console.SetText("");
				}

				if (_lua_pcall(x4_LuaState, 0, LUA_MULTRET, 0) != LUA_OK)
					{
					console.SetText(_lua_tolstring(x4_LuaState, -1, 0));
				}
			}
			else
			{
				console.SetText(_lua_tolstring(x4_LuaState, -1, 0));
			}
		}
		ig::SameLine();
		if (ig::Button("Clear Console", ImVec2(SetHeightPercent(0.50f), SetHeightPercent(0.10f))))
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
				// The window is too small or collapsed.
				return false;
			}

			menuWidth = view.x;
			menuHeight = view.y;

			//Render();

			return true;
		}

		return true;
	}

}
