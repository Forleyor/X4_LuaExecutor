#include <filesystem>
#include <format>
#include <sstream>
#include "imgui.h"
#include "imgui_impl_win32.h"

#include "menu.h"
#include "../../utils/utils.h"
#include "../lua.h"

namespace Menu
{
    LuaType GetLuaType(const std::string &type)
    {
        if (type == "nil")
            return LuaType::nil;
        if (type == "boolean")
            return LuaType::boolean;
        if (type == "lightuserdata")
            return LuaType::lightuserdata;
        if (type == "number")
            return LuaType::number;
        if (type == "string")
            return LuaType::string;
        if (type == "table")
            return LuaType::table;
        if (type == "function")
            return LuaType::function;
        if (type == "userdata")
            return LuaType::userdata;
        if (type == "thread")
            return LuaType::thread;
        if (type == "cdata")
            return LuaType::cdata;

        return LuaType::unknown;
    }

    void ParseTable(const std::string &text, bool isGlobalTable = false)
    {
        LuaTable luaTable;
        if (isGlobalTable)
        {
            luaTable.parentTable = "NONE",
            luaTable.name = "_G";
        }

        LuaTableEntry currentEntry = {
        .key = "",
        .typeString = "",
        .luaType = LuaType::unknown,
        .address = nullptr,
        };

        std::string tableEntryText = "";

        std::istringstream ss(text);
        while (std::getline(ss, tableEntryText))
        {
            std::istringstream ssline(tableEntryText);

            ssline >> currentEntry.key;

            ssline >> currentEntry.typeString;
            currentEntry.typeString.erase(currentEntry.typeString.length() - 1);
            currentEntry.luaType = GetLuaType(currentEntry.typeString);

            ssline >> currentEntry.address;

            printf("key: %s\n", currentEntry.key.c_str());
            printf("address: %p\n", currentEntry.address);
            printf("type: %d (%s)\n", currentEntry.luaType, currentEntry.typeString.c_str());
            //printf("[%s] = %d: %p (%s)\n", currentEntry.key.c_str(), currentEntry.luaType, currentEntry.address, currentEntry.typeString.c_str());

            luaTable.entries.push_back(currentEntry);
        }

        luaTables.push_back(luaTable);
        processTable = false;
    }

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
        io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16, &font_config);

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

    void ShowLuaExecutor()
    {
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
    }

    void ShowTableViewer()
    {
        std::string entryText;
        for (const auto& table : luaTables)
        {
            for (auto entry : table.entries)
            {
                entryText = std::format("[{0}] = {1}: {2} ({1})", entry.key, entry.typeString, entry.address);
                if (entry.luaType == LuaType::table)
                {
                    if (ImGui::TreeNodeEx(entryText.c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
                    {
                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGui::Text(entryText.c_str());
                }
            }
        }
    }

    void Render()
    {
        if (!bShowMenu)
            return;
        
        ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_FirstUseEver);
        ImGui::Begin("Lua Tools");
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        if (HandleWindowResize() == false)
        {
            ImGui::End();
            return;
        }

        //ImGui::ShowDemoWindow();

        if (ImGui::BeginTabBar("tabs"))
        {
            if (ImGui::BeginTabItem("Code Editor"))
            {
                ShowLuaExecutor();
                mode = LuaExecutor;
                ImGui::EndTabItem();

            }
            //if (ImGui::BeginTabItem("Table Viewer"))
            //{
            //    mode = TableViewer;
            //    ShowTableViewer();
            //    ImGui::EndTabItem();
            //}
            ImGui::EndTabBar();
        }

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
