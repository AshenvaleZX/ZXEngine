require = function(modname)
    CustomRequire(modname)
end

require("Utils")
require("Math")
require("GameLogicMgr")
require("EventMgr")
require("EngineEventMgr")
require("EngineEnum")

Log("Lua init, version: %s", _VERSION)
LogWarning("Log Warning Test")
LogError("Log Error Test")