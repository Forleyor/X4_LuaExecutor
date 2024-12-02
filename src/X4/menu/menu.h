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
    inline bool autoExectute = false;
    inline TextEditor editor;
	inline TextEditor console;
	inline float gameWidth;
    inline float gameHeight;
    inline float menuWidth;
    inline float menuHeight;
} // namespace Menu
