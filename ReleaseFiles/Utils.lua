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