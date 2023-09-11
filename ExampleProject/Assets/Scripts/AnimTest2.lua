local AnimTest2 = NewGameLogic()

function AnimTest2:Start()
    self.animator = self.gameObject:GetComponent("Animator")
    self.animator:Play("Idle")
end

function AnimTest2:Update()

end

return AnimTest2