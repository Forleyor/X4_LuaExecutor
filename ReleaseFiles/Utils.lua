function LogTable(itable, filter)
    local str = ""
    local isFilter = false
    local line
    if filter == nil then
        str = "Table:\n"
    else
        isFilter = true
        str = "Table with Filter: " .. filter .. "\n"
    end

    for k,v in pairs(itable) do
        line = "["..k.."] = "..tostring(v).." ("..type(v)..")\n"
        if #line + #str >= 8192 then
            DebugError(str)
            if isFilter then
                if type(v) == filter then
                    str = line
                end
            else
                str = line
            end
        else
            if isFilter then
                if type(v) == filter then
                    str = str .. line
                end
            else
                str = str .. line
            end
        end
    end
    DebugError(str)
end

function PrintTable(itable, filter)
    local str = ""
    local isFilter = false
    local line
    if filter == nil then
        str = "Table:\n"
    else
        isFilter = true
        str = "Table with Filter: " .. filter .. "\n"
    end

    for k,v in pairs(itable) do
        line = "["..k.."] = "..tostring(v).." ("..type(v)..")\n"
        if isFilter then
            if type(v) == filter then
                str = str .. line
            end
        else
            str = str .. line
        end
    end
    print(str)
end

function PrintMenus()
	print("Menus:")
	for i, menu in ipairs(Menus) do
		print(string.format("  Menu[%d]: %s", i, menu.name))
	end
end

function FindMenu(name)
	for i, menu in ipairs(Menus) do
		if menu.name == name then
			print(string.format("Menu[%d]: %s", i, menu.name))
		end
	end
end

function GetMenu(name)
	for i, menu in ipairs(Menus) do
		if menu.name == name then
			return menu
		end
	end
end

function PrintComponentData(component, ...)
    local args = {...}
    local properties = {}

    for _,v in pairs(args) do
        properties[v] = GetComponentData(component, v)
    end

    local keys = {}
    for key in pairs(properties) do
        table.insert(keys, key)
    end
    table.sort(keys)

    print("Component Data:")
    for _, key in pairs(keys) do
       print(string.format("  %s: %s", key, properties[key]))
    end
end
