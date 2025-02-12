#include <cstdio>
#include <mutex>
#include <thread>
#include <filesystem>

#include <imgui.h>
#include <MinHook.h>
#include <INIReader.h>

#include "hooks.h"
#include "hook_vulkan.h"
#include "../X4/menu/menu.h"
#include "../utils/utils.h"
#include "../X4/lua.h"



static HWND g_hWindow = NULL;
static std::mutex g_mReinitHooksGuard;

static UINT_PTR menuHotkey;

static DWORD WINAPI ReinitializeGraphicalHooks(LPVOID lpParam) {
    std::lock_guard<std::mutex> guard{g_mReinitHooksGuard};

    printf("[!] Hooks will reinitialize!\n");

    HWND hNewWindow = U::GetProcessWindow( );
    while (hNewWindow == reinterpret_cast<HWND>(lpParam)) {
        hNewWindow = U::GetProcessWindow( );
    }

    H::bShuttingDown = true;

    H::Free( );
    H::Init( );

    H::bShuttingDown = false;
    Menu::bShowMenu = true;

    return 0;
}

static WNDPROC oWndProc;
static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN)
    {
        if (wParam == menuHotkey)
        {
            Menu::bShowMenu = !Menu::bShowMenu;
            return 0;
        }
    } else if (uMsg == WM_DESTROY)
    {
        HANDLE hHandle = CreateThread(NULL, 0, ReinitializeGraphicalHooks, hWnd, 0, NULL);
        if (hHandle != NULL)
            CloseHandle(hHandle);
    }

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (Menu::bShowMenu) {
        
         return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) == 0;
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

namespace Hooks {
    void Init( ) {

		std::filesystem::path iniPath = std::filesystem::current_path() / "plugins" / "LuaExecutor" / "LuaExecutor.ini";
        INIReader config(iniPath.string());

        if (config.ParseError() == -1)
            printf("Error! Failed to parse LuaExecutor.ini!");

        menuHotkey = config.GetInteger("Hotkeys", "OpenMenu", VK_INSERT);
        printf("LuaExecutor Open Menu default hotkey is \"0x2D (INSERT)\", current assignment is 0x%02x\n", (char*)menuHotkey);

        g_hWindow = U::GetProcessWindow( );
        VK::Hook(g_hWindow);
        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
    }

    void Free( ) {
        if (oWndProc) {
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
        }

        MH_DisableHook(MH_ALL_HOOKS);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        VK::Unhook( );
    }
} // namespace Hooks
