local tools = {}
---Rounds
---@param x number Int or number to round
---@param n integer the amount of int or decimals to round to, if wanting int use 0
---@return number rounded number
function tools.round(x, n)
    local factor = 10 ^ n
    return math.floor(x * factor + 0.5) / factor
end

return tools
