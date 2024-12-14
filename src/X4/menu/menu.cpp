#include <filesystem>

#include "imgui.h"
#include "imgui_impl_win32.h"

#include "menu.h"
#include "../../utils/utils.h"
#include "../lua.h"

namespace Menu
{
    void InitializeContext(HWND hwnd) {
        if (ImGui::GetCurrentContext())
            return;

        RECT window;
        GetWindowRect(Utils::GetProcessWindow(), &window);
        
        gameWidth = static_cast<float>(window.right);
        gameHeight = static_cast<float>(window.bottom);
        menuWidth = gameWidth / 2;
        menuHeight = gameHeight / 2;

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = io.LogFilename = nullptr;
        io.FontAllowUserScaling = true;
        
        ImFontConfig font_config;
        std::filesystem::path fontPath = std::filesystem::current_path() / "plugins" / "LuaExecutor" / "Fonts" / "CascadiaMono.ttf";
        io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 24, &font_config);

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
        
        ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_FirstUseEver);
        ImGui::Begin("Lua Executor");

        if (HandleWindowResize() == false)
        {
            ImGui::End();
            return;
    }

        editor.Render("LuaEditor", ImVec2(menuWidth, SetHeightPercent(0.45f)), true);
        if (autoExectute && editor.IsTextChanged() || ImGui::Button("Execute", ImVec2(250.0f, 50.0f)))
        {
            Lua::code = editor.GetText();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Console", ImVec2(250.0f, 50.0f)))
        {
            console.SetText("");
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto Execute", &autoExectute);

        console.Render("Console", ImVec2(menuWidth, SetHeightPercent(0.45f)), true);

        ImGui::End();
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
