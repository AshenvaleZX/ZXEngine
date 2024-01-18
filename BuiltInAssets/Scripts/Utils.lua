Utils = {}

function Utils.StringSplit(oriStr, p)
    if not oriStr or not p then
        return {}
    end
	local nFindStartIndex = 1
	local nSplitIndex = 1
	local nSplitArray = {}
	while true do
		local nFindLastIndex = string.find(oriStr, p, nFindStartIndex)
		if not nFindLastIndex then
			nSplitArray[nSplitIndex] = string.sub(oriStr, nFindStartIndex, string.len(oriStr))
			break
		end
		nSplitArray[nSplitIndex] = string.sub(oriStr, nFindStartIndex, nFindLastIndex - 1)
		nFindStartIndex = nFindLastIndex + string.len(p)
		nSplitIndex = nSplitIndex + 1
	end
	return nSplitArray
end

local logType = 0
local function TypeLog(msg)
    if logType == 1 then
        Debug.Log(msg)
    elseif logType == 2 then
        Debug.LogWarning(msg)
    elseif logType == 3 then
        Debug.LogError(msg)
    else
        print(msg)
    end
end

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
        TypeLog('printTable: nil table')
        return
    end

    local depth = depth or 1
    if depth > 9 then
        TypeLog('too many depth; ignore')
        return
    end

    TypeLog(string.format('%s{', string.rep('    ', depth - 1)))
    for k, v in pairs(t) do
        if type(v) == 'table' then
            TypeLog(string.format('%s[%s] = ', string.rep('    ', depth), GetKeyValueStr(k, depth + 1)))
            GetTableStr(v, depth + 1)
            TypeLog(',')
        else
            TypeLog(string.format('%s[%s] = %s,', string.rep('    ', depth), GetKeyValueStr(k, depth + 1), GetKeyValueStr(v, depth + 1)))
        end
    end
    TypeLog(string.rep('    ', depth - 1) .. '}')
end

local function fprint(s, ...)
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
                TypeLog(string.format(subs, table.unpack(data)))
                data = {}
            end
            start = argLocal[i] + 2
            GetTableStr(sarg)
            if i == length then
                TypeLog(string.sub(s, start, -1))
            end
        else
            sarg = tostring(sarg)
            table.insert(data, sarg)
        end
    end

    if #argLocal > 0 and argLocal[length] - 1 >= 0 then
        local subs = string.sub(s, start, -1)
        TypeLog(string.format(subs, table.unpack(data)))
    elseif #argLocal == 0 then
        TypeLog(s)
    end
end

function Log(s, ...)
    logType = 1
    fprint(s, ...)
end

function LogWarning(s, ...)
    logType = 2
    fprint(s, ...)
end

function LogError(s, ...)
    logType = 3
    fprint(s, ...)
end