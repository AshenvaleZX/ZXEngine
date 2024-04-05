local RandomMove = NewGameLogic()

function RandomMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
    self.originPos = self.trans:GetPosition()
    self.moveType = math.random(2)

    if self.moveType == 1 then
        self.angle = math.random() * 3.1415
        self.radius = math.random(2, 4)
    else
        self.lineDis = math.random(2, 4)

        local rX = math.random()
        local rY = math.random()
        local dir = Math.NormalizeVec2({ x = rX, y = rY})
        self.lineDir = { x = dir.x, y = 0, z = dir.y }
    end
end

function RandomMove:Update()
    if self.moveType == 1 then
        self:CircleMove()
    else
        self:LineMove()
    end
end

-- 绕圈
RandomMove.angle = 0
RandomMove.radius = 20
function RandomMove:CircleMove()
    self.angle = self.angle + Time.GetDeltaTime()
    local x = math.sin(self.angle) * self.radius
    local z = math.cos(self.angle) * self.radius
    self.trans:SetPosition(self.originPos.x + x, self.originPos.y, self.originPos.z + z)
end

-- 直线
RandomMove.lineSpeed = 2
RandomMove.curLineDis = 0
RandomMove.lineDirForward = true
function RandomMove:LineMove()
    if self.lineDirForward then
        self.curLineDis = self.curLineDis + self.lineSpeed * Time.GetDeltaTime()
        if self.curLineDis > self.lineDis then
            self.lineDirForward = false
        end
    else
        self.curLineDis = self.curLineDis - self.lineSpeed * Time.GetDeltaTime()
        if self.curLineDis < -self.lineDis then
            self.lineDirForward = true
        end
    end

    local offset = Math.MulVec3Num(self.lineDir, self.curLineDis)
    self.trans:SetPosition(self.originPos.x + offset.x, self.originPos.y, self.originPos.z + offset.z)
end

return RandomMove