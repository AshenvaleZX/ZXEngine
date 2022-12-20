local resFile = io.open(arg[1].."/tmp.txt", "r")
local total = 0
for line in resFile:lines() do
    total = total + tonumber(line)
end
print(total)