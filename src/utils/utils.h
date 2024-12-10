#pragma once

#include <windows.h>

enum RenderingBackend_t {
    NONE = 0,
    VULKAN,
};

namespace Utils {
    HWND GetProcessWindow( );
    void UnloadDLL( );
    
    HMODULE GetCurrentImageBase( );

    int GetCorrectDXGIFormat(int eCurrentFormat);
}

namespace U = Utils;
