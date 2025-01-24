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