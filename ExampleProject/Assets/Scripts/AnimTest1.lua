local AnimTest1 = NewGameLogic()

function AnimTest1:Start()
    self.animator = self.gameObject:GetComponent("Animator")
    self.animator:Play("Defeat")
end

function AnimTest1:Update()

end

return AnimTest1