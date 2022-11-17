Event = {}
Event.events = {}

-- 注册Lua事件
function Event:AddEventHandler(event, handler)
    if not event or type(event) ~= "string" then
        fprint("Event parameter in addlistener function has to be string, %s not right.", type(event))
        return
    end
    if not handler or type(handler) ~= "function" then
        fprint("Handler parameter in addlistener function has to be function, %s not right", type(handler))
        return
    end
    if not self.events[event] then
        self.events[event] = {}
    end
    self.events[event][handler] = true
end

-- 触发Lua事件
function Event:FireEvent(event, ...)
    if self.events[event] then
        for handler,_ in pairs(self.events[event])do
            handler(...)
        end
    end
end

-- 移除Lua事件函数
function Event:RemoveEventHandler(event, handler)
    if self.events[event] then
        self.events[event][handler] = nil
    end
end

-- 移除整个Lua事件的所有函数
function Event:ClearEventHandler(event)
    if self.events[event] then
        self.events[event] = nil
    end
end

-- 清空Lua事件注册函数
function Event:ClearAllHandler()
    self.events={}
end