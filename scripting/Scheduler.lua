local Scheduler = {
    tasks = {}
}

function Wait(seconds)
    coroutine.yield(seconds + 0.01)
end

function Scheduler:update(dt)
    for i = #self.tasks, 1, -1 do
        local task = self.tasks[i]
        task.delay = task.delay - dt
        if task.delay <= 0 then
            local ok, delay = coroutine.resume(task.co)
            if coroutine.status(task.co) == "dead" then
                table.remove(self.tasks, i)
            else
                task.delay = delay or 0
            end
        end
    end
end

function Scheduler:run(co)
    local ok, delay = coroutine.resume(co)
    if coroutine.status(co) ~= "dead" then
        table.insert(self.tasks, { co = co, delay = delay or 0 })
    end
end

return Scheduler
