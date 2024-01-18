local path = Resources.GetAssetsPath().."/Scripts/?.lua"
package.path = package.path..";"..path

require("Utils")
require("Math")
require("GameLogicMgr")
require("EventMgr")
require("EngineEventMgr")

Log("Lua init, version: %s", _VERSION)
LogWarning("Log Warning Test")
LogError("Log Error Test")