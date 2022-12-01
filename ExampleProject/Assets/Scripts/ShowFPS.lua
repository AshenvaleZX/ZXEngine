local ShowFPS = NewGameLogic()

-- 累计帧数
ShowFPS.frame = 0
-- 更新间隔
ShowFPS.interval = 0.1
-- 累计时间
ShowFPS.accumulation = 0
-- 累计FPS之和
ShowFPS.accumulateFPS = 0

function ShowFPS:Start()
    self.FPSText = self.gameObject:GetComponent("UITextRenderer")
end

function ShowFPS:Update()
    local dt = Time.GetDeltaTime()
    local fps = 1 / dt
    self.frame = self.frame + 1
    self.accumulation = self.accumulation + dt
    self.accumulateFPS = self.accumulateFPS + fps

    -- 0.1秒更新一次FPS显示
    if self.accumulation >= self.interval then
        self.FPSText:SetText("FPS: " .. math.floor(self.accumulateFPS / self.frame))
        self.frame = 0
        self.accumulation = 0
        self.accumulateFPS = 0
    end
end

return ShowFPS