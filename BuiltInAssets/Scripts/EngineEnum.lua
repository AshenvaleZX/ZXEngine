TouchPhase = {}
TouchPhase_Enum = 
{
    "Began",
    "Moved",
    "Ended",
}

for i, key in ipairs(TouchPhase_Enum) do
    TouchPhase[key] = i - 1
end

PlatformType = {}
PlatformType_Enum = 
{
    "Windows",
    "MacOS",
    "Linux",
    "Android",
    "iOS",
}

for i, key in ipairs(PlatformType_Enum) do
    PlatformType[key] = i - 1
end