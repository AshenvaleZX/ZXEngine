local LogicTest = NewGameLogic()

LogicTest.radius = 5
LogicTest.angle = 0
LogicTest.rot = 0

function LogicTest:Start()
    self.trans = self.GameLogic:GetTransform()
end

function LogicTest:Update()
    self.angle = self.angle + 0.5 * Time.GetDeltaTime()
    local x = math.sin(self.angle)*self.radius
    local z = math.cos(self.angle)*self.radius
    self.trans:SetPosition(x, 0, z)

    self.rot = self.rot + 5 * Time.GetDeltaTime()
    if self.rot > 360 then
        self.rot = self.rot - 360
    end
    self.trans:SetEulerAngles(0, 0, self.rot)
end

return LogicTest