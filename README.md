# LuaExecutor
Execute Lua directly in X4  

## Notes
After loading a save open the map menu once and the lua state should be initialized and ready.  

INSERT key to open menu  
CTRL + Scrollwheel zooms in/out the imgui window and or text editors (Depending on where the mouse is focused)  

## Build Dependancies
Uses Visual Studio with vcpkg integration (manifest) for these:
- MinHook
- Dear ImGui
  - win32-binding
  - vulkan-bindings

Also requires Vulkan SDK
