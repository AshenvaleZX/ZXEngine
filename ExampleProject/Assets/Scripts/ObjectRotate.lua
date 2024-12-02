local ObjectRotate = NewGameLogic()

ObjectRotate.angle = 0
ObjectRotate.speed = 1

function ObjectRotate:Start()
    self.trans = self.gameObject:GetComponent("Transform")
end

function ObjectRotate:Update()
    self.angle = self.angle + self.speed * Time.GetDeltaTime()
    self.trans:SetEulerAngles(0, math.deg(self.angle), 0)
end

return ObjectRotate