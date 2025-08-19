local engine = {}
engine.nextScene = nil
local scheduler = require("Scheduler")
local scenes = require("scenes")
engine.Buttons = {
    UP = 26,
    LEFT = 4,
    DOWN = 22,
    RIGHT = 7,
    A = 44,
    B = 27,
}
engine.currentScene = {}
engine.sceneChange = false

--#region Tools

engine.Tools = {}
local tools = {}
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
---Updates the internal input system in lua so you don't have to call keydown, just pressed, etc.
function engine.Input.Update()
    for _, button in pairs(engine.Buttons) do
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


--#region Audio
engine.Audio = {}
function engine.Audio.PlaySfxOneShot(soundName, volume)
    cAudio.PlaySfx(soundName, volume)
end

function engine.Audio.PlayBGM(soundName, volume)
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


---Registers functions that will run by the engine for different things.
---@param typeNumber integer The actual type that corresponds to the first number of type in tiled
---@param funcTable table array table with the functions | Create, Start, Update, Destroy
function engine.RegisterGameObjectFunctions(typeNumber, funcTable)
    if #funcTable ~= 4 then engine.Log.LogWarn("bad number of function args passed to go func register") end
    for index, value in ipairs(funcTable) do
        if type(value) ~= "nil" and type(value) ~= "function" then
            engine.Log.LogWarn(("Invalid function at index %d for type %d"):format(index, typeNumber))
        end
    end
    cGameObject.NewGameObjectType(typeNumber, funcTable)
end

function engine.LoadTilemap(mapname)
    cScene.LoadMap(mapname)
end

function engine.LoadTilemapObjects()
    cScene.LoadObjectsOnMap()
end

---Sets all current gameobjects to be destroyed the next time DestroyGameObjects is called
---@param force boolean|nil default false | If set to true, even if DoNotDestroy is set on the gameobject, set it to be destroyed..
function engine.SetGameObjectsToBeDestroyed(force)
    if force == nil then
        force = false
    end
    cGameObject.SetDestroyGameObjects(force)
end

---Destroys all gameobjects that have ToBeDestroyed set on them.
function engine.DestroyGameObjects()
    cGameObject.DestroyGameObjects()
end

function engine.LoadSceneCo(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    return coroutine.create(function()
        if fadeInTimeSec > 0 then
            engine.FadeoutScreen(fadeInTimeSec)
            Wait(fadeInTimeSec)
        end
        engine.LoadTilemap(mapname)
        engine.SetGameObjectsToBeDestroyed(false)
        engine.LoadTilemapObjects()
        engine.DestroyGameObjects()
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
            engine.Log.LogWarn("Loading " .. name)
            local success, testui = pcall(require, name)
            if success then
                ui.CreatePanelFromTable(testui)
            else
                engine.Log.LogError("Failed to load UI: " .. name .. " — " .. tostring(testui))
            end
        end

        if fadeOutTimeSec > 0 then
            engine.FadeinScreen(fadeOutTimeSec)
            Wait(fadeOutTimeSec)
        end
        if bgm ~= nil then engine.Audio.PlayBGM(bgm, volume) end
        engine.sceneChange = false
    end)
end

function engine.LoadSceneEx(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    if uiname == "" then uiname = nil end
    local co = engine.LoadSceneCo(mapname, uiname, bgm, volume, fadeInTimeSec, fadeOutTimeSec)
    scheduler:run(co)
end

function engine.LoadScene(mapKey)
    engine.nextScene = mapKey
end

function engine.LoadDefaultScene()
    local defaultScene = scenes["default"]
    local sceneTable = scenes.scenes[defaultScene]
    local co = engine.LoadSceneCo(sceneTable[1], sceneTable[2], sceneTable[3], sceneTable[4], sceneTable[5],
        sceneTable[6])
    scheduler:run(co)
end

function engine.SetWindowOptions(windowSize, windowHeight, windowName)
    cEngine.SetWindowOptions(windowSize, windowHeight, windowName)
end

function engine.SetScalingOptions(worldWidth, WorldHeight)
    cEngine.SetScalingOptions(worldWidth, WorldHeight)
end

function engine.FadeoutScreen(fadeTime)
    cEffects.Fadeout(fadeTime)
end

function engine.FadeinScreen(fadeTime)
    cEffects.Fadein(fadeTime)
end

--- Is the screen fading?  Use this to track when the screen is fading
---@return boolean true if screen is fading.
function engine.IsScreenFading()
    return cEffects.IsScreenFading
end

function engine.SetInputFunc(func)
    cEngine.SetInputFunc(func)
end

function engine.SetUpdateFunc(func)
    cEngine.SetUpdateFunc(func)
end

function engine.SetDrawFunc(func)
    cEngine.SetDrawFunc(func)
end

function engine.DrawRect(rect)
    cEngine.DrawRect(rect.x, rect.y, rect.w, rect.h)
end

function engine.DrawRectCamOffset(rect)
    cEngine.DrawRectCamOffset(rect.x, rect.y, rect.w, rect.h)
end

function engine.DeltaTimeInSeconds()
    return cEngine.DeltaTimeSeconds
end

function engine.DeltaTimeMS()
    return cEngine.DeltaTimeMilliseconds
end

function engine.GameTicks()
    return cEngine.Ticks
end

--Is the game running on a mobile platform.
function engine.IsMobile()
    return cEngine.IsMobile
end

---comment
---@param name string The name of the animator to load, without the fileextension
---@return integer animator handle
function engine.CreateAnimator(name, spritePtr)
    return cAnimation.CreateAnimator(name, spritePtr)
end

function engine.PlayAnimation(animator, animationName)
    return cAnimation.PlayAnimation(animator, animationName)
end

---comment
---@param animator integer anim handle
---@param animatorSpeed number the speed that this animator should play, 0.0 - 5.0
---@return unknown
function engine.SetAnimatorSpeed(animator, animatorSpeed)
    return cAnimation.SetAnimatorSpeed(animator, animatorSpeed)
end

function engine.DestroyAnimator(animator)
    return cAnimation.DestroyAnimator(animator)
end

function engine.SetCameraFollowTarget(gameobject)
    return cCamera.SetCameraFollowTarget(gameobject)
end

function engine.CheckGameobjectForCollision(gameobject)
    return cGameObject.CheckSolids(gameobject)
end

function engine.CheckRectForCollision(rectTable)
    local rect = engine.Tools.NormalizeRect(rectTable)
    return cGameObject.CheckSolidsRect(rect)
end

function engine.CheckForCollision(a, b)
    return a.x < b.x + b.w and
        a.x + a.w > b.x and
        a.y < b.y + b.h and
        a.y + a.h > b.y
end

function engine.MapName()
    return cEngine.MapName()
end

return engine
