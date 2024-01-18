Math = {}

function Math.Clamp(num, min, max)
    return math.max(math.min(num, max), min)
end

function Math.Lerp(from, to, t)
    return from + (to - from) * t
end

function Math.LerpVec3(from, to, t)
    return { x = Math.Lerp(from.x, to.x, t), y = Math.Lerp(from.y, to.y, t), z = Math.Lerp(from.z, to.z, t) }
end

function Math.AddVec3(v1, v2)
    return { x = v1.x + v2.x, y = v1.y + v2.y, z = v1.z + v2.z }
end

function Math.SubVec3(v1, v2)
    return { x = v1.x - v2.x, y = v1.y - v2.y, z = v1.z - v2.z }
end

function Math.MulVec3(v1, v2)
    return { x = v1.x * v2.x, y = v1.y * v2.y, z = v1.z * v2.z }
end

function Math.DivVec3(v1, v2)
    return { x = v1.x / v2.x, y = v1.y / v2.y, z = v1.z / v2.z }
end

function Math.AddVec3Num(v, num)
    return { x = v.x + num, y = v.y + num, z = v.z + num }
end

function Math.SubVec3Num(v, num)
    return { x = v.x - num, y = v.y - num, z = v.z - num }
end

function Math.MulVec3Num(v, num)
    return { x = v.x * num, y = v.y * num, z = v.z * num }
end

function Math.DivVec3Num(v, num)
    return { x = v.x / num, y = v.y / num, z = v.z / num }
end

function Math.DotVec3(v1, v2)
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z
end

function Math.CrossVec3(v1, v2)
    return { x = v1.y * v2.z - v1.z * v2.y, y = v1.z * v2.x - v1.x * v2.z, z = v1.x * v2.y - v1.y * v2.x }
end

function Math.MagnitudeVec3(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end