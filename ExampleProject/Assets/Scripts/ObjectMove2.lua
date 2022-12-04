local ObjectMove2 = NewGameLogic()

ObjectMove2.radius = 20
ObjectMove2.angle = 0
ObjectMove2.rot = 0

function ObjectMove2:Start()
    self.trans = self.gameObject:GetComponent("Transform")
end

function ObjectMove2:Update()
    self.angle = self.angle - 1 * Time.GetDeltaTime()
    local x = math.sin(self.angle)*self.radius
    local z = math.cos(self.angle)*self.radius
    self.trans:SetPosition(x, 10, z)
end

return ObjectMove2