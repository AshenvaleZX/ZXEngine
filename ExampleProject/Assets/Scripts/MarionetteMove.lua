local MarionetteMove = NewGameLogic()

MarionetteMove.speed = 1
MarionetteMove.velocity = { x = 0, y = 0, z = 0 }
MarionetteMove.force = { x = 0, y = 117.6, z = 0 }

-- true:自动控制人偶
-- false:WSAD手动控制人偶
MarionetteMove.isAutoMove = true
MarionetteMove.curTime = 6.5

function MarionetteMove:Start()
    self.rigidBody = GameObject.Find("Marionette/Head"):GetComponent("RigidBody")

    if not self.isAutoMove then
        EngineEvent:AddEventHandler(EngineEventType.KEY_W_DOWN, self.MoveZ_P, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_S_DOWN, self.MoveZ_N, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_A_DOWN, self.MoveX_N, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_D_DOWN, self.MoveX_P, self)

        EngineEvent:AddEventHandler(EngineEventType.KEY_W_UP, self.MoveZ_E, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_S_UP, self.MoveZ_E, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_A_UP, self.MoveX_E, self)
        EngineEvent:AddEventHandler(EngineEventType.KEY_D_UP, self.MoveX_E, self)
    end
end

function MarionetteMove:Update()
    if self.isAutoMove then
        self.curTime = self.curTime + Time.GetDeltaTime()
        if self.curTime > 8 then
            self.curTime = self.curTime - 8
        end

        local step = math.floor(self.curTime)
        if step == 0 then
            self:MoveZ_P()
        elseif step == 2 then
            self:MoveX_P()
        elseif step == 4 then
            self:MoveZ_N()
        elseif step == 6 then
            self:MoveX_N()
        else
            self:MoveZ_E()
            self:MoveX_E()
        end
    end
end

function MarionetteMove:FixedUpdate()
    self.rigidBody:AddForce(self.force)
    self.rigidBody:SetVelocity(self.velocity)
end

function MarionetteMove:MoveZ_P()
    self.velocity.z = self.speed
end

function MarionetteMove:MoveZ_N()
    self.velocity.z = -self.speed
end

function MarionetteMove:MoveX_P()
    self.velocity.x = self.speed
end

function MarionetteMove:MoveX_N()
    self.velocity.x = -self.speed
end

function MarionetteMove:MoveZ_E()
    self.velocity.z = 0
end

function MarionetteMove:MoveX_E()
    self.velocity.x = 0
end

return MarionetteMove