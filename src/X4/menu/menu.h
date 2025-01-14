#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include "Addons/TextEditor.h"

namespace Menu
{
    inline enum Mode
    {
        None = 0,
        LuaExecutor,
        TableViewer
    } mode;

    enum class LuaType
    {
        unknown = -1,
        nil = 0,
        boolean,
        lightuserdata,
        number,
        string,
        table,
        function,
        userdata,
        thread,
        cdata
    };

    struct LuaTableEntry
    {
        std::string key;
        std::string typeString;
        LuaType luaType;
        void* address;
        bool isExpandedTable;
    };

    struct LuaTable
    {
        std::string parentTable;
        std::string name;
        std::vector<LuaTableEntry> entries;
    };

    void InitializeContext(HWND hwnd);
    void Render( );
    bool HandleWindowResize();
    void ParseTable(const std::string& text, bool isGlobalTable);

    inline bool bShowMenu = false;
    inline bool updateConsole = false;
    inline bool autoExectute = false;
    inline int currentSize = 0;
    inline TextEditor editor;
    inline TextEditor console;
    inline float gameWidth;
    inline float gameHeight;
    inline float menuWidth;
    inline float menuHeight;
    inline float buttonHeight;
    inline bool isTableView;
    inline std::vector<LuaTable> luaTables;
    inline bool processTable = true;
    
} // namespace Menu
