#pragma once

#include <Windows.h>
#include "Addons/TextEditor.h"

namespace Menu {
    void InitializeContext(HWND hwnd);
    void Render( );
    bool HandleWindowResize();

    inline bool bShowMenu = false;
	inline bool updateConsole = false;
    inline bool autoExectute = false;
    inline TextEditor editor;
	inline TextEditor console;
	inline float gameWidth;
    inline float gameHeight;
    inline float menuWidth;
	inline float menuHeight;
	inline float buttonHeight;
} // namespace Menu
