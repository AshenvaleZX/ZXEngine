require("EngineEventType")
EngineEvent = {}
EngineEvent.events = {}

-- 注册C++引擎事件
function EngineEvent:AddEventHandler(id, handler, table)
    if not id or type(id) ~= "number" then
        LogError("EngineEvent parameter in addlistener function has to be number, %s not right.", type(id))
        return
    end
    if not handler or type(handler) ~= "function" then
        LogError("Handler parameter in addlistener function has to be function, %s not right", type(handler))
        return
    end
    if not self.events[id] then
        self.events[id] = {}
    end
    self.events[id][handler] = {table = table}
end

-- 接收并广播从C++引擎传过来的事件（这个函数不应该在Lua代码里调用）
function EngineEvent:FireEvent(arg)
    local s,e = string.find(arg, ",")
    local id = tonumber(string.sub(arg, 1, s-1))
    local msg = string.sub(arg, e+1)

    if self.events[id] then
        for handler, data in pairs(self.events[id])do
            if data.table then
                handler(data.table, msg)
            else
                handler(msg)
            end
        end
    end
end

-- 移除C++引擎事件函数
function EngineEvent:RemoveEventHandler(id, handler)
    if self.events[id] then
        self.events[id][handler] = nil
    end
end

-- 移除整个C++引擎事件的所有函数
function EngineEvent:ClearEventHandler(id)
    if self.events[id] then
        self.events[id] = nil
    end
end

-- 清空C++引擎事件注册函数
function EngineEvent:ClearAllHandler()
    self.events={}
end