AllGameLogic = {}

local ID = 0
local function GetNextID()
    ID = ID + 1
    return ID
end

function NewGameLogic()
    local GameLogic = {}
    GameLogic._ID = GetNextID()
    AllGameLogic[GameLogic._ID] = GameLogic
    return GameLogic
end