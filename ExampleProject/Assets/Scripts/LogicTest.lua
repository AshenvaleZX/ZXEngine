local LogicTest = NewGameLogic()

function LogicTest:Start()
    fprint("Start in lua %s", self._ID)
end

function LogicTest:Update()
    fprint("Update in lua %s", self._ID)
end

return LogicTest