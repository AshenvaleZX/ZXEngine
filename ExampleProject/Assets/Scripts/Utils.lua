local function GetKeyValueStr(v, depth)
    if type(v) == 'string' then
        return string.format('%q', v)
    elseif type(v) == 'number' then
        return tostring(v)
    elseif type(v) == 'boolean' then
        return ((v and 'true') or 'false')
    elseif type(v) == 'table' then
        return 'table'
    elseif type(v) == 'userdata' then
        return 'userdata'
    elseif type(v) == 'function' then
        return 'function'
    else
        return 'invalid type ' .. type(v)
    end
end

local function GetTableStr(t, depth)
    if t == nil then
        Debug.Log('printTable: nil table')
        return
    end

    local depth = depth or 1
    if depth > 9 then
        Debug.Log('too many depth; ignore')
        return
    end

    Debug.Log(string.format('%s{', string.rep('    ', depth - 1)))
    for k, v in pairs(t) do
        if type(v) == 'table' then
            Debug.Log(string.format('%s[%s] = ', string.rep('    ', depth), GetKeyValueStr(k, depth + 1)))
            GetTableStr(v, depth + 1)
            Debug.Log(',')
        else
            Debug.Log(string.format('%s[%s] = %s,', string.rep('    ', depth), GetKeyValueStr(k, depth + 1), GetKeyValueStr(v, depth + 1)))
        end
    end
    Debug.Log(string.rep('    ', depth - 1) .. '}')
end

function fprint(s, ...)
    if type(s) ~= 'string' then
        return
    end
    
    local data = {}
    local argLocal = {}
    local start = 1
    local count = 0
    while true do
        local s1, s2 = string.find(s, '%%s', start)
        if s2 then
            start = s2 + 1
            table.insert(argLocal, s1)
        else
            break
        end
        count = count + 1
    end

    start = 1
    local length = select('#', ...)

    if length ~= count then
        return
    end

    for i = 1, length do
        local sarg = select(i, ...)
        if type(sarg) == 'table' then
            if argLocal[i] - 1 > 0 then
                local subs = string.sub(s, start, argLocal[i] - 1)
                Debug.Log(string.format(subs, table.unpack(data)))
                data = {}
            end
            start = argLocal[i] + 2
            GetTableStr(sarg)
            if i == length then
                Debug.Log(string.sub(s, start, -1))
            end
        else
            sarg = tostring(sarg)
            table.insert(data, sarg)
        end
    end

    if #argLocal > 0 and argLocal[length] - 1 >= 0 then
        local subs = string.sub(s, start, -1)
        Debug.Log(string.format(subs, table.unpack(data)))
    elseif #argLocal == 0 then
        Debug.Log(s)
    end
end