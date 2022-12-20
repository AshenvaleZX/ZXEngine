local codeFile = io.open(arg[1]..arg[2], "r")
local count = 0
for line in codeFile:lines() do
    count = count + 1
end
local resFile = io.open(arg[3].."/tmp.txt", "a")
resFile:write(count.."\n")