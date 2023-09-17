local AnimRoleMove = NewGameLogic()

AnimRoleMove.curAnim = "Walk"

AnimRoleMove.curTime = 1.5
AnimRoleMove.blendTime = 1.5
AnimRoleMove.switchTime = 4.0

AnimRoleMove.radius = 12
AnimRoleMove.speed = 2.3
AnimRoleMove.walkSpeed = 2.3
AnimRoleMove.runSpeed = 10
AnimRoleMove.curAngle = 0

function AnimRoleMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
    self.animator = self.gameObject:GetComponent("Animator")
    self.animator:Play("Walk")
end

function AnimRoleMove:Lerp(from, to, t)
    return from + (to - from) * t
end

function AnimRoleMove:Update()
    -- 动画切换
    self.curTime = self.curTime - Time.GetDeltaTime()
    if self.curTime < 0 then
        if self.curAnim == "Walk" then
            self.animator:Switch("Run", self.blendTime)
            self.curAnim = "Run"
        else
            self.animator:Switch("Walk", self.blendTime)
            self.curAnim = "Walk"
        end
        self.curTime = self.switchTime
    end

    -- 速度
    if self.curTime < self.switchTime - self.blendTime then
        self.speed = self.curAnim == "Walk" and self.walkSpeed or self.runSpeed
    else
        local curSpeed = self.runSpeed
        local targetSpeed = self.walkSpeed
        if self.curAnim == "Run" then
            curSpeed = self.walkSpeed
            targetSpeed = self.runSpeed
        end
        self.speed = self:Lerp(curSpeed, targetSpeed, (self.switchTime - self.curTime) / self.blendTime)
    end

    -- 位移
    local moveDis = self.speed * Time.GetDeltaTime()
    local deltaAngle = moveDis / self.radius
    self.curAngle = self.curAngle + deltaAngle
    if self.curAngle > 2 * math.pi then
        self.curAngle = self.curAngle - 2 * math.pi
    end

    local x = math.sin(self.curAngle) * self.radius
    local z = math.cos(self.curAngle) * self.radius
    self.trans:SetPosition(x, -3, z)

    -- 朝向
    self.trans:SetEulerAngles(0, math.deg(self.curAngle) + 90, 0)
end

return AnimRoleMove