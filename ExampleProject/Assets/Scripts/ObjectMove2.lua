local LogicTest = NewGameLogic()

LogicTest.radius = 20
LogicTest.angle = 0
LogicTest.rot = 0

function LogicTest:Start()
    self.trans = self.GameLogic:GetTransform()
end

function LogicTest:Update()
    self.angle = self.angle - 0.2 * Time.GetDeltaTime()
    local x = math.sin(self.angle)*self.radius
    local z = math.cos(self.angle)*self.radius
    self.trans:SetPosition(x, 10, z)
end

return LogicTest