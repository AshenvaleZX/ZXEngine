local ObjectMove = NewGameLogic()

ObjectMove.radius = 20
ObjectMove.angle = 0
ObjectMove.rot = 0

function ObjectMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
end

function ObjectMove:Update()
    self.angle = self.angle + 2 * Time.GetDeltaTime()
    local x = math.sin(self.angle)*self.radius
    local z = math.cos(self.angle)*self.radius
    self.trans:SetPosition(x, 0, z)

    self.rot = self.rot + 50 * Time.GetDeltaTime()
    if self.rot > 360 then
        self.rot = self.rot - 360
    end
    self.trans:SetEulerAngles(0, 0, self.rot)
end

return ObjectMove