local engine = {}
local scenes = require("scenes")
local gamestate = require("gameState")

--#region Coroutine
engine.Coroutine = {
    tasks = {}
}

-- Adds a wait to a coroutine, the coroutine will wait the amount of seconds during the coroutine update
function engine.Coroutine.Wait(seconds)
    seconds = tonumber(seconds) or 0
    coroutine.yield(math.max(0.01, seconds))
end

function engine.Coroutine.update()
    for i = #engine.Coroutine.tasks, 1, -1 do
        local task = engine.Coroutine.tasks[i]
        task.delay = task.delay - gamestate.DeltaTimeSeconds


        if task.delay <= 0 then
            local ok, delayOrErr = coroutine.resume(task.co)
            if not ok then
                print("Scheduler coroutine error: " .. tostring(delayOrErr))
                table.remove(engine.Coroutine.tasks, i)
            elseif coroutine.status(task.co) == "dead" then
                table.remove(engine.Coroutine.tasks, i)
            else
                -- Make sure resumed value is a valid number
                local delay = tonumber(delayOrErr) or 0
                task.delay = math.max(0.01, delay)
            end
        end
    end
end

-- function engine.Coroutine.run(co)

--     assert(type(co) == "thread", "Scheduler:run expects a coroutine")

--     local ok, delayOrErr = coroutine.resume(co)
--     if not ok then
--         print("Scheduler coroutine error on run: " .. tostring(delayOrErr))
--         return
--     end
--     if coroutine.status(co) ~= "dead" then
--         local delay = tonumber(delayOrErr) or 0
--         table.insert(engine.Coroutine.tasks, {
--             co = co,
--             delay = math.max(0.01, delay)
--         })
--     end
-- end

---Starts a coroutine, adds it to be updated in the engine update and will handle waits appropriately.
---@param co any
function engine.Coroutine.run(co)
    engine.Log.LogDebug("Coroutine.run called, type=" .. tostring(type(co)) ..
        " status=" .. (type(co) == "thread" and coroutine.status(co) or "n/a"))
    assert(type(co) == "thread", "Coroutine:run expects a coroutine")
    table.insert(engine.Coroutine.tasks, {
        co = co,
        delay = 0.01 -- start next frame
    })
    engine.Log.LogDebug("Task count now " .. tostring(#engine.Coroutine.tasks))
end

--#endregion Coroutine

--#region Tools

engine.Tools = {}
---Rounds
---@param x number Int or number to round
---@param n integer the amount of int or decimals to round to, if wanting int use 0
---@return number rounded number
function engine.Tools.Round(x, n)
    local factor = 10 ^ n
    return math.floor(x * factor + 0.5) / factor
end

function engine.Tools.NormalizeArrayTableWithKeys(rect, keys)
    if rect and #rect == #keys and rect[keys[1]] == nil then
        local normalizedRect = {}
        for i = 1, #keys, 1 do
            normalizedRect[keys[i]] = rect[i]
        end
        return normalizedRect
    end
    return rect
end

function engine.Tools.NormalizeRect(rect)
    return engine.Tools.NormalizeArrayTableWithKeys(rect, { "x", "y", "w", "h" })
end

--#endregion Tools

--#region Sprite
engine.Sprite = {}
engine.Sprite.Sprites = {}
function engine.Sprite.NewSprite(imageName, parentPtr, textureSrcRectTable, offsetSizeRectTable)
    textureSrcRectTable = engine.Tools.NormalizeRect(textureSrcRectTable)
    offsetSizeRectTable = engine.Tools.NormalizeRect(offsetSizeRectTable)
    return cSprite.NewSprite(imageName, parentPtr, textureSrcRectTable, offsetSizeRectTable)
end

function engine.Sprite.DestroySprite(spritePtr)
    return cSprite.DestroySprite(spritePtr)
end

--#endregion Sprite

--#region Input
engine.Input = {}
engine.Input.Buttons = {
    UP = 26,
    LEFT = 4,
    DOWN = 22,
    RIGHT = 7,
    A = 44,
    B = 27,
}
engine.Input.ControllerOverlayUpdateFunc = nil
engine.Input.UIButtonPresses = {
    JustPressed = {},
    JustReleased = {},
    Down = {}
}
function engine.Input.KeyboardKeyJustPressed(key)
    return engine.Input.UIButtonPresses.JustPressed[key] or cInput.IsKeyboardKeyPressed(key)
end

function engine.Input.KeyboardKeyDown(key)
    return engine.Input.UIButtonPresses.Down[key] or cInput.IsKeyboardKeyDown(key)
end

engine.Input._UIButtonLastState = {}
engine.Input.UIButtonThisFrame = {}
---Updates the internal input system in lua so you don't have to call keydown, just pressed, etc.
function engine.Input.Update()
    for _, button in pairs(engine.Input.Buttons) do
        local wasDown = engine.Input._UIButtonLastState[button] or false
        local isDown = engine.Input.UIButtonThisFrame[button] or false

        engine.Input.UIButtonPresses.Down[button] = isDown
        engine.Input.UIButtonPresses.JustPressed[button] = (not wasDown) and isDown
        engine.Input.UIButtonPresses.JustReleased[button] = wasDown and (not isDown)

        engine.Input._UIButtonLastState[button] = isDown
    end

    -- Clear current frame state for next Update
    engine.Input.UIButtonThisFrame = {}
end

--#endregion Input

--#region Log
engine.Log = {}
function engine.Log.LogDebug(message)
    cLog.Log(message, 1)
end

function engine.Log.LogInfo(message)
    cLog.Log(message, 2)
end

function engine.Log.LogWarn(message)
    cLog.Log(message, 3)
end

function engine.Log.LogError(message)
    cLog.Log(message, 4)
end

function engine.Log.LogCritical(message)
    cLog.Log(message, 5)
end

--#endregion Log

--#region GameObject

engine.Gameobject = {}

function engine.Gameobject.Position(ptr)
    return cGameObject.Position(ptr)
end

function engine.Gameobject.SetPosition(ptr, x, y)
    return cGameObject.SetPosition(ptr, x, y)
end

function engine.Gameobject.Size(ptr)
    return cGameObject.Size(ptr)
end

function engine.Gameobject.SetSize(ptr, w, h)
    return cGameObject.SetSize(ptr, w, h)
end

function engine.Gameobject.Sprite(ptr)
    return engine.Sprite.Sprites[ptr]
end

-- Calls the create function for the gameobject below ..
function engine.Gameobject.CreateGameObjectInCurrentMap()
    return cGameObject.CreateGameObject()
end

---Sets the gameobject type
---@param ptr userdata go ptr
---@param type integer The number of gameobject type, so it can update, start correctly
---@return nil
function engine.Gameobject.SetType(ptr, type)
    return cGameObject.SetType(ptr, type)
end

---Sets an object name, useful in debug builds for debugging purposes
---@param ptr any
---@param name any
function engine.Gameobject.SetName(ptr, name)
    cGameObject.SetName(ptr, name)
end

---Registers functions that will run by the engine for different things.
---@param typeNumber integer The actual type that corresponds to the first number of type in tiled
---@param funcTable table array table with the functions | Create, Start, Update, Destroy
function engine.Gameobject.RegisterGameObjectFunctions(typeNumber, funcTable)
    if #funcTable ~= 4 then engine.Log.LogWarn("bad number of function args passed to go func register") end
    for index, value in ipairs(funcTable) do
        if type(value) ~= "nil" and type(value) ~= "function" then
            engine.Log.LogWarn(("Invalid function at index %d for type %d"):format(index, typeNumber))
        end
    end
    cGameObject.NewGameObjectType(typeNumber, funcTable)
end

---Sets all current gameobjects to be destroyed the next time DestroyGameObjects is called
---@param force boolean|nil default false | If set to true, even if DoNotDestroy is set on the gameobject, set it to be destroyed..
function engine.Gameobject.SetGameObjectsToBeDestroyed(force)
    if force == nil then
        force = false
    end
    cGameObject.SetDestroyGameObjects(force)
end

---Destroys all gameobjects that have ToBeDestroyed set on them.
function engine.Gameobject.DestroyGameObjects()
    cGameObject.DestroyGameObjects()
end

--#endregion GameObject

--#region Audio
engine.Audio = {}
function engine.Audio.PlaySfxOneShot(soundName, volume)
    cAudio.PlaySfx(soundName, volume)
end

function engine.Audio.PlayBGM(soundName, volume)
    volume = volume or 1.0
    cAudio.PlayBgm(soundName, volume, 0)
end

---Loads and Plays a BGM in the background slot
function engine.Audio.PlayBGMBackground(soundName, volume)
    cAudio.PlayBgm(soundName, volume, 1)
end

function engine.Audio.UnPauseBGMBackground()
    cAudio.UnPauseBGMBackground(1)
end

--- Stops a BGM in the background slot, does not unload but moves back to start
function engine.Audio.StopBGMBackground()
    cAudio.StopBgm(1)
end

function engine.Audio.SetGlobalBGMVolume(volume)
    cAudio.SetGlobalBgmVolume(volume)
end

function engine.Audio.SetGlobalSFXVolume(volume)
    cAudio.SetGlobalSfxVolume(volume)
end

--#endregion Audio

--#region Map
engine.Map = {}
function engine.Map.LoadTilemap(mapname)
    cScene.LoadMap(mapname)
end

function engine.Map.LoadTilemapObjects()
    cScene.LoadObjectsOnMap()
end

function engine.MapName()
    return cEngine.MapName()
end

--#endregion Map

--#region Scene
engine.Scene = {}
function engine.Scene.LoadSceneCo(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    return coroutine.create(function()
        if fadeInTimeSec > 0 then
            engine.Effects.FadeoutScreen(fadeInTimeSec)
            engine.Coroutine.Wait(fadeInTimeSec)
        end
        engine.Map.LoadTilemap(mapname)
        engine.Gameobject.SetGameObjectsToBeDestroyed(false)
        engine.Map.LoadTilemapObjects()
        engine.Gameobject.DestroyGameObjects()
        local ui = require("UI")
        -- Destroy all ui panels that are not donotdestroy.
        for key, value in pairs(ui.UIInstance) do
            if value.data ~= nil and value.doNotDestroy == false then
                engine.Log.LogWarn("Destroying panel " .. key)
                ui.DestroyPanel(value)
                ui.UIInstance[key] = nil
                value = nil
            end
        end
        -- Load ui if needed
        if uiname ~= nil then
            local name = "ui/" .. uiname
            engine.Log.LogDebug("Loading " .. name)
            local success, testui = pcall(require, name)
            if success then
                ui.CreateUIFromUIFile(testui)
            else
                engine.Log.LogError("Failed to load UI: " .. name .. " — " .. tostring(testui))
            end
        end

        if fadeOutTimeSec > 0 then
            engine.Effects.FadeinScreen(fadeOutTimeSec)
            engine.Coroutine.Wait(fadeOutTimeSec)
        end
        if bgm ~= nil then engine.Audio.PlayBGM(bgm, volume) end
        gamestate.sceneChange = false
    end)
end

function engine.Scene.LoadSceneEx(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    if uiname == "" then uiname = nil end
    local co = engine.Scene.LoadSceneCo(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    engine.Coroutine.run(co)
end

function engine.Scene.LoadScene(mapKey)
    gamestate.nextScene = mapKey
end

function engine.Scene.LoadDefaultScene()
    local defaultScene = scenes["default"]
    local sceneTable = scenes.scenes[defaultScene]
    local co = engine.Scene.LoadSceneCo(sceneTable[1], sceneTable[2], sceneTable[3], sceneTable[4], sceneTable[5],
        sceneTable[6])
    engine.Coroutine.run(co)
end

--#endregion Scene

--#region Window
engine.Window = {}
function engine.Window.SetWindowOptions(windowSize, windowHeight, windowName)
    cEngine.SetWindowOptions(windowSize, windowHeight, windowName)
end

function engine.Window.SetScalingOptions(worldWidth, WorldHeight)
    cEngine.SetScalingOptions(worldWidth, WorldHeight)
end

--#endregion Window

--#region Effects
engine.Effects = {}
function engine.Effects.FadeoutScreen(fadeTime)
    cEffects.Fadeout(fadeTime)
end

function engine.Effects.FadeinScreen(fadeTime)
    cEffects.Fadein(fadeTime)
end

--#endregion Effects

--#region Draw
engine.Draw = {}

function engine.Draw.DrawRect(rect)
    cEngine.DrawRect(rect.x, rect.y, rect.w, rect.h)
end

function engine.Draw.DrawRectCamOffset(rect)
    cEngine.DrawRectCamOffset(rect.x, rect.y, rect.w, rect.h)
end

--#endregion Draw

--#region Animation
engine.Animation = {}
---comment
---@param name string The name of the animator to load, without the fileextension
---@return integer animator handle
function engine.Animation.CreateAnimator(name, spritePtr)
    return cAnimation.CreateAnimator(name, spritePtr)
end

function engine.Animation.PlayAnimation(animator, animationName, loops)
    loops = loops or -1
    return cAnimation.PlayAnimation(animator, animationName, loops)
end

function engine.Animation.AddAnimationToQueue(animator, animationName, loops)
    loops = loops or -1
    return cAnimation.AddAnimationToAnimatorQueue(animator, animationName, loops)
end

---comment
---@param animator integer anim handle
---@param animatorSpeed number the speed that this animator should play, 0.0 - 5.0
---@return unknown
function engine.Animation.SetAnimatorSpeed(animator, animatorSpeed)
    return cAnimation.SetAnimatorSpeed(animator, animatorSpeed)
end

function engine.Animation.DestroyAnimator(animator)
    return cAnimation.DestroyAnimator(animator)
end

--#endregion Animation

--#region Camera
engine.Camera = {}
function engine.Camera.SetCameraFollowTarget(gameobject)
    return cCamera.SetCameraFollowTarget(gameobject)
end

--#endregion Camera

--#region Collision

engine.Collision = {}
function engine.Collision.CheckGameobjectForCollision(gameobject)
    return cGameObject.CheckSolids(gameobject)
end

function engine.Collision.CheckRectForCollision(rectTable)
    local rect = engine.Tools.NormalizeRect(rectTable)
    return cGameObject.CheckSolidsRect(rect)
end

function engine.Collision.CheckForCollision(a, b)
    return a.x < b.x + b.w and
        a.x + a.w > b.x and
        a.y < b.y + b.h and
        a.y + a.h > b.y
end

--#endregion Collision

--#region Time
---Get the deltatime seconds from the engine.. this is cached every update frame in gamestate.DeltaTimeSeconds
---@return number Time The amount in seconds of time since the last frame
engine.Time = {}
function engine.Time.DeltaTimeInSeconds()
    return cEngine.DeltaTimeSeconds
end

---Get the deltatime ms from the engine.. this is cached every update frame in gamestate.DeltaTimeSeconds
---@return integer Time The amount in ms of time since the last frame
function engine.Time.DeltaTimeMS()
    return cEngine.DeltaTimeMilliseconds
end

function engine.Time.GameTicks()
    return cEngine.Ticks
end

--#endregion Time
--#region Core
function engine.SetInputFunc(func)
    cEngine.SetInputFunc(func)
end

function engine.SetUpdateFunc(func)
    cEngine.SetUpdateFunc(func)
end

function engine.SetDrawFunc(func)
    cEngine.SetDrawFunc(func)
end

--Should be called in your update function, updates the coroutines that use wait, etc.
function engine.EngineUpdate()
    gamestate.DeltaTimeSeconds = engine.Time.DeltaTimeInSeconds()
    gamestate.DeltaTimeMS = engine.Time.DeltaTimeMS()
    engine.Coroutine.update()
end

--Is the game running on a mobile platform.
function engine.IsMobile()
    return cEngine.IsMobile
end

--- Is the screen fading?  Use this to track when the screen is fading
---@return boolean true if screen is fading.
function engine.IsScreenFading()
    return cEffects.IsScreenFading
end

--#endregion Core

return engine
