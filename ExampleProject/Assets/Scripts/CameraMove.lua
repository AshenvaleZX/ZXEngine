local CameraMove = NewGameLogic()

CameraMove.MovementSpeed = 2.5

function CameraMove:Start()
    self.trans = self.GameLogic:GetTransform()
    EngineEvent:AddEventHandler(EngineEventType.KEY_W_PRESS, self.MoveForwardCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_S_PRESS, self.MoveBackCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_A_PRESS, self.MoveLeftCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_D_PRESS, self.MoveRightCallBack, self)
end

function CameraMove:Update()

end

function CameraMove:MoveForwardCallBack()
    self:MoveCamera("Forward")
end

function CameraMove:MoveBackCallBack()
    self:MoveCamera("Back")
end

function CameraMove:MoveRightCallBack()
    self:MoveCamera("Right")
end

function CameraMove:MoveLeftCallBack()
    self:MoveCamera("Left")
end

function CameraMove:MoveCamera(dirName)
    local dir = {}
    if dirName == "Forward" then
        dir = self.trans:GetForward()
    elseif dirName == "Back" then
        dir = self.trans:GetForward()
        dir = {x = -dir.x, y = -dir.y, z = -dir.z}
    elseif dirName == "Right" then
        dir = self.trans:GetRight()
    elseif dirName == "Left" then
        dir = self.trans:GetRight()
        dir = {x = -dir.x, y = -dir.y, z = -dir.z}
    else
        return
    end

    local velocity = self.MovementSpeed * Time.GetDeltaTime()
    local pos = self.trans:GetPosition()
    pos = 
    {
        x = pos.x + dir.x * velocity,
        y = pos.y + dir.y * velocity,
        z = pos.z + dir.z * velocity,
    }
    self.trans:SetPosition(pos.x, pos.y, pos.z)
end

return CameraMove