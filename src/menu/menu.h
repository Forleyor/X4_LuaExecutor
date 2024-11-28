#pragma once

#include <Windows.h>
#include "Addons/TextEditor.h"

namespace Menu {
    void InitializeContext(HWND hwnd);
    void Render( );
    bool HandleWindowResize();

    inline bool bShowMenu = false;
    inline void *x4_LuaState = nullptr;
	inline bool updateConsole = false;
    inline TextEditor editor;
	inline TextEditor console;
	inline float width = 1200.0f;
    inline float height = 800.0f;
    inline bool autoExectute = false;
} // namespace Menu
