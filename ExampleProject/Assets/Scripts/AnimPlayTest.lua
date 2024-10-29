local AnimPlayTest = NewGameLogic()

AnimPlayTest.animationName = ""

function AnimPlayTest:Start()
    self.animator = self.gameObject:GetComponent("Animator")
    self.animator:Play(self.animationName)
end

function AnimPlayTest:Update()

end

return AnimPlayTest