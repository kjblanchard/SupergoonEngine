-- local Scheduler = {
--     tasks = {}
-- }

-- function Wait(seconds)
--     coroutine.yield(seconds + 0.01)
-- end

-- function Scheduler:update(dt)
--     for i = #self.tasks, 1, -1 do
--         local task = self.tasks[i]
--         task.delay = task.delay - dt
--         if task.delay <= 0 then
--             local ok, delay = coroutine.resume(task.co)
--             if coroutine.status(task.co) == "dead" then
--                 table.remove(self.tasks, i)
--             else
--                 task.delay = delay or 0
--             end
--         end
--     end
-- end

-- function Scheduler:run(co)
--     local ok, delay = coroutine.resume(co)
--     if coroutine.status(co) ~= "dead" then
--         table.insert(self.tasks, { co = co, delay = delay or 0 })
--     end
-- end

-- return Scheduler

local Scheduler = {
    tasks = {}
}

-- Ensure at least 0.01s delay to prevent immediate resumption
function Wait(seconds)
    seconds = tonumber(seconds) or 0
    coroutine.yield(math.max(0.01, seconds))
end

function Scheduler:update(dt)
    for i = #self.tasks, 1, -1 do
        local task = self.tasks[i]
        task.delay = task.delay - dt

        if task.delay <= 0 then
            local ok, delayOrErr = coroutine.resume(task.co)

            if not ok then
                print("Scheduler coroutine error: " .. tostring(delayOrErr))
                table.remove(self.tasks, i)
            elseif coroutine.status(task.co) == "dead" then
                table.remove(self.tasks, i)
            else
                -- Make sure resumed value is a valid number
                local delay = tonumber(delayOrErr) or 0
                task.delay = math.max(0.01, delay)
            end
        end
    end
end

function Scheduler:run(co)
    assert(type(co) == "thread", "Scheduler:run expects a coroutine")

    local ok, delayOrErr = coroutine.resume(co)

    if not ok then
        print("Scheduler coroutine error on run: " .. tostring(delayOrErr))
        return
    end

    if coroutine.status(co) ~= "dead" then
        local delay = tonumber(delayOrErr) or 0
        table.insert(self.tasks, {
            co = co,
            delay = math.max(0.01, delay)
        })
    end
end

return Scheduler
