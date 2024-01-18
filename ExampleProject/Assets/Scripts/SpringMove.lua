local SpringMove = NewGameLogic()

function SpringMove:Start()
    self.trans = self.gameObject:GetComponent("Transform")
    self.originScale = self.trans:GetLocalScale()

    self.jointObj1 = GameObject.Find("Spring/JointObj1")
    self.jointTrans1 = self.jointObj1:GetComponent("Transform")
    self.jointObj2 = GameObject.Find("Spring/JointObj2")
    self.jointTrans2 = self.jointObj2:GetComponent("Transform")
end

function SpringMove:Update()
    local pos1 = self.jointTrans1:GetLocalPosition()
    local pos2 = self.jointTrans2:GetLocalPosition()
    self.trans:SetLocalPosition((pos1.x + pos2.x) / 2, (pos1.y + pos2.y) / 2, (pos1.z + pos2.z) / 2)
    
    local dis = Math.MagnitudeVec3(Math.SubVec3(pos1, pos2))
    self.trans:SetLocalScale(self.originScale.x, dis * 0.4, self.originScale.z)
end

return SpringMove