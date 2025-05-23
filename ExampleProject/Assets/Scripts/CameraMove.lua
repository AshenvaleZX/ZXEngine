local CameraMove = NewGameLogic()

CameraMove.firstMouse = true
CameraMove.MouseSensitivity = 0.1
CameraMove.MovementSpeed = 25

CameraMove.LastTouchDis = 0
CameraMove.IsFirstTouchZoom = true

function CameraMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
    EngineEvent:AddEventHandler(EngineEventType.MOUSE_BUTTON_2_DOWN, self.RegisterCameraControl, self)
    EngineEvent:AddEventHandler(EngineEventType.MOUSE_BUTTON_2_UP, self.UnregisterCameraControl, self)
end

function CameraMove:Update()
    if GlobalData.Platform == PlatformType.Android or GlobalData.Platform == PlatformType.iOS then
        self:UpdateTouch()
    end
end

function CameraMove:RegisterCameraControl()
    self.firstMouse = true
    InputManager.ShowCursor(false)
    EngineEvent:AddEventHandler(EngineEventType.UPDATE_MOUSE_POS, self.MouseMoveCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_W_PRESS, self.MoveForwardCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_S_PRESS, self.MoveBackCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_A_PRESS, self.MoveLeftCallBack, self)
    EngineEvent:AddEventHandler(EngineEventType.KEY_D_PRESS, self.MoveRightCallBack, self)
end

function CameraMove:UnregisterCameraControl()
    InputManager.ShowCursor(true)
    EngineEvent:RemoveEventHandler(EngineEventType.UPDATE_MOUSE_POS, self.MouseMoveCallBack)
    EngineEvent:RemoveEventHandler(EngineEventType.KEY_W_PRESS, self.MoveForwardCallBack)
    EngineEvent:RemoveEventHandler(EngineEventType.KEY_S_PRESS, self.MoveBackCallBack)
    EngineEvent:RemoveEventHandler(EngineEventType.KEY_A_PRESS, self.MoveLeftCallBack)
    EngineEvent:RemoveEventHandler(EngineEventType.KEY_D_PRESS, self.MoveRightCallBack)
end

function CameraMove:UpdateTouch()
    local touchCount = InputManager.GetTouchCount()
    if touchCount == 1 then
        local touch = InputManager.GetTouch(0)
        if touch.phase == TouchPhase.Began then
            self.firstMouse = true
        elseif touch.phase == TouchPhase.Moved then
            self:MouseMove(touch.x, touch.y)
        end
        self.IsFirstTouchZoom = true
    elseif touchCount == 2 then
        local touch1 = InputManager.GetTouch(0)
        local touch2 = InputManager.GetTouch(1)
        if self.IsFirstTouchZoom then
            self.LastTouchDis = math.sqrt((touch1.x - touch2.x) * (touch1.x - touch2.x) + (touch1.y - touch2.y) * (touch1.y - touch2.y))
            self.IsFirstTouchZoom = false
        else
            local touchDis = math.sqrt((touch1.x - touch2.x) * (touch1.x - touch2.x) + (touch1.y - touch2.y) * (touch1.y - touch2.y))
            local delta = touchDis - self.LastTouchDis

            self.MovementSpeed = 100
            if delta > 0 then
                self:MoveCamera("Forward")
            else
                self:MoveCamera("Back")
            end
        end
        self.firstMouse = true
    else
        self.firstMouse = true
        self.IsFirstTouchZoom = true
    end
end

function CameraMove:MouseMoveCallBack(args)
    local argList = Utils.StringSplit(args, '|')
    local xpos = tonumber(argList[1])
    local ypos = tonumber(argList[2])
    self:MouseMove(xpos, ypos)
end

function CameraMove:MouseMove(xpos, ypos)
    if self.firstMouse then
        self.lastX = xpos
        self.lastY = ypos
        self.firstMouse = false
    end

    local xoffset = xpos - self.lastX
    local yoffset = self.lastY - ypos

    self.lastX = xpos
    self.lastY = ypos

    self:RotateAngleOfView(xoffset, yoffset)
end

function CameraMove:RotateAngleOfView(horizontalOffset, verticalOffset)
    horizontalOffset = horizontalOffset * self.MouseSensitivity
    verticalOffset = verticalOffset * self.MouseSensitivity

    local eulerAngle = self.trans:GetEulerAngles()
    eulerAngle.x = eulerAngle.x - verticalOffset
    eulerAngle.x = Math.Clamp(eulerAngle.x, -89, 89)
    eulerAngle.y = eulerAngle.y + horizontalOffset
    self.trans:SetEulerAngles(eulerAngle.x, eulerAngle.y, eulerAngle.z)
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