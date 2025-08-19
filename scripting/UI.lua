-- UI.lua
local UI = {}
local engine = require("Engine")
UI.UIInstance = {}
UI.lookup = {}
local font = ""
local fontSize = 0

local function makeKey(parentKey, name)
    if parentKey == nil or parentKey == "" then
        return name
    else
        return parentKey .. "." .. name
    end
end
local function normalizeArrayTableWithKeys(rect, keys)
    if rect and #rect == #keys and rect[keys[1]] == nil then
        local normalizedRect = {}
        for i = 1, #keys, 1 do
            normalizedRect[keys[i]] = rect[i]
        end
        return normalizedRect
    end
    return rect
end

local function normalizeRect(rect)
    return normalizeArrayTableWithKeys(rect, { "x", "y", "w", "h" })
end

local function normalizeColorRect(rect)
    return normalizeArrayTableWithKeys(rect, { "r", "g", "b", "a" })
end

function UI.GetObjectSize(objectPtr)
    return cUI.GetObjectSize(objectPtr)
end

function UI.GetObjectLocation(objectPtr)
    return cUI.GetObjectLocation(objectPtr)
end

function UI.SetObjectLocation(objectPtr, x, y)
    return cUI.SetObjectLocation(objectPtr, x, y)
end

function UI.SetObjectVisible(objectPtr, isVisible)
    return cUI.SetObjectVisible(objectPtr, isVisible)
end

local function CreatePanel(name, rect, parentPanel)
    rect = normalizeRect(rect)
    return cUI.CreatePanel(name, rect, parentPanel)
end

local function CreateImage(name, rect, parentPanel, filename, srcRect, transparencyInt)
    rect = normalizeRect(rect)
    srcRect = normalizeRect(srcRect)
    transparencyInt = transparencyInt or 255
    return cUI.CreateImage(name, rect, parentPanel, filename, srcRect, transparencyInt)
end

local function CreateImageAnimator(name, rect, parentPanel, filename, srcRect, transparencyInt, defaultAnim)
    rect = normalizeRect(rect)
    srcRect = normalizeRect(srcRect)
    transparencyInt = transparencyInt or 255
    local animData = cUI.CreateImageAnimator(name, rect, parentPanel, filename, srcRect, transparencyInt)
    if defaultAnim then
        UI.PlayAnimation(animData, defaultAnim)
    end
    return animData
end

local function Create9SliceImage(name, rect, parentPanel, filename, color)
    rect = normalizeRect(rect)
    color = normalizeColorRect(color)
    return cUI.Create9SliceImage(name, rect, parentPanel, filename, color)
end

local function CreateText(name, rect, parentPanel, text, fontName, fontSize, centerX, centerY, wordwrap, color)
    rect = normalizeRect(rect)
    color = normalizeColorRect(color)
    return cUI.CreateText(name, rect, parentPanel, text, fontName, fontSize, centerX, centerY, wordwrap, color)
end

local function CreateRect(name, rect, parentPanel, color)
    rect = normalizeRect(rect)
    color = normalizeColorRect(color)
    return cUI.CreateRect(name, rect, parentPanel, color)
end

local function CreateHLG(name, rect, parentPanel, spacing)
    rect = normalizeRect(rect)
    return cUI.CreateLayoutGroup(name, rect, parentPanel, spacing, true)
end

local function CreateVLG(name, rect, parentPanel, spacing)
    rect = normalizeRect(rect)
    return cUI.CreateLayoutGroup(name, rect, parentPanel, spacing, false)
end

local function CreateButton(name, rect, parentPanel, pressedFunc, hoverFunc, pressOnRelease)
    rect = normalizeRect(rect)
    return cUI.CreateButton(name, rect, parentPanel, { pressedFunc, hoverFunc }, pressOnRelease)
end

local function CreateProgressBar(name, rect, parentPanel, color)
    rect = normalizeRect(rect)
    color = normalizeColorRect(color)
    return cUI.CreateProgressBar(name, rect, parentPanel, color)
end

function UI.UpdateProgressBarPercent(ptr, percent)
    return cUI.UpdateProgressBarPercent(ptr, percent)
end

local function CreateUIObjectAndChildren(objTable, parentPtr, parentTable, parentKey)
    if objTable.isMobile and not engine.IsMobile() then return end
    -- TODO we should validate these so it doesn't break
    local node = { data = nil, children = {} }
    if objTable.type == "image" then
        node.data = CreateImage(objTable.name, objTable.location, parentPtr, objTable.imageName, objTable.srcRect,
            objTable.transparency)
    elseif objTable.type == "imageAnimator" then
        node.data = CreateImageAnimator(objTable.name, objTable.location, parentPtr, objTable.imageName, objTable
            .srcRect, objTable.transparency, objTable.defaultAnim)
    elseif objTable.type == "9slice" then
        node.data = Create9SliceImage(objTable.name, objTable.location, parentPtr, objTable.imageName, objTable.color)
    elseif objTable.type == "text" then
        local thisFont = font
        local thisSize = fontSize
        if objTable.font ~= nil then
            thisFont = objTable.font
            font = thisFont
        end
        if objTable.centeredX == nil then objTable.centeredX = false end
        if objTable.centeredY == nil then objTable.centeredY = false end
        if objTable.wordWrap == nil then objTable.wordWrap = false end
        if objTable.color == nil then objTable.color = { 255, 255, 255, 255 } end
        if objTable.size ~= nil then
            thisSize = objTable.size
            fontSize = thisSize
        end
        node.data = CreateText(objTable.name, objTable.location, parentPtr, objTable.text, thisFont, thisSize,
            objTable.centeredX, objTable.centeredY, objTable.wordWrap, objTable.color)
    elseif objTable.type == "rect" then
        node.data = CreateRect(objTable.name, objTable.location, parentPtr, objTable.color)
    elseif objTable.type == "progressBar" then
        node.data = CreateProgressBar(objTable.name, objTable.location, parentPtr, objTable.color)
    elseif objTable.type == "hlg" then
        node.data = CreateHLG(objTable.name, objTable.location, parentPtr, objTable.spacing)
    elseif objTable.type == "vlg" then
        node.data = CreateVLG(objTable.name, objTable.location, parentPtr, objTable.spacing)
    elseif objTable.type == "button" then
        node.data = CreateButton(objTable.name, objTable.location, parentPtr, objTable.pressedFunc, objTable.hoverFunc,
            objTable.pressOnRelease)
    elseif objTable.type == "panel" then
        node.data = CreatePanel(objTable.name, objTable.location, parentPtr)
    end
    if objTable.userdata ~= nil then node["userdata"] = objTable.userdata end
    if objTable.visible ~= nil and objTable.visible ~= true then UI.SetObjectVisible(node.data, false) end
    parentTable[objTable.name] = node

    local key = makeKey(parentKey, objTable.name)
    UI.lookup[key] = node


    if objTable.children then
        for _, child in ipairs(objTable.children) do
            CreateUIObjectAndChildren(child, node.data, node.children, key)
        end
    end
    return node.data
end
local function destroyLuaDataForPanelRecursive(panel)
    if not panel or panel.doNotDestroy then
        return
    end

    -- Destroy children first
    if panel.children then
        for i, child in pairs(panel.children) do
            destroyLuaDataForPanelRecursive(child)
            -- Only clear reference if child was destroyed
            if not (child and child.doNotDestroy) then
                panel.children[i] = nil
            end
        end
    end

    -- Destroy the C-side object for this panel
    if panel.data then
        cUI.DestroyUIObject(panel.data)
        panel.data = nil
    end

    -- Clear all Lua keys to allow GC
    for k in pairs(panel) do
        panel[k] = nil
    end
end

function UI.DestroyPanel(panelTable)
    if not panelTable then return end

    -- Recursively destroy
    destroyLuaDataForPanelRecursive(panelTable)

    -- Remove from UI.UIInstance
    for key, value in pairs(UI.UIInstance) do
        if value == panelTable then
            UI.UIInstance[key] = nil
        end
    end

    panelTable = nil
end

---Update the text of a ui text.
---@param textData userdata the ui object ptr
---@param text string the string
function UI.UpdateText(textData, text)
    cUI.UpdateText(textData, text)
end

---Update the text Size of a ui text.
---@param textData userdata the ui object ptr
---@param size integer Size to set it to
function UI.SetTextSize(textData, size)
    cUI.SetTextSize(textData, size)
end

---Play animation of UI object
---@param animData userdata the ui object ptr
---@param animName string the anim name
function UI.PlayAnimation(animData, animName)
    cUI.PlayAnimation(animData, animName)
end

function UI.UpdateNumLettersForText(textData, numLetters)
    cUI.TextSetNumLetters(textData, numLetters)
end

function UI.CreatePanelFromTable(table)
    -- Handle creating this UI element only on specific platforms.
    if table.isMobile and not engine.IsMobile() then return end
    if UI.UIInstance[table.name] ~= nil then
        engine.Log.LogWarn("Trying to create a panel that already exists in root, not creating a new one!")
        return
    end
    -- Top level is always a panel
    local doNotDestroy = table.doNotDestroy or false
    local root = { data = CreatePanel(table.name, { 0, 0, 0, 0 }, nil), children = {}, doNotDestroy = doNotDestroy }
    UI.UIInstance[table.name] = root
    for _, child in ipairs(table.children) do
        CreateUIObjectAndChildren(child, root.data, root.children, "")
    end
    if table.startFunc ~= nil then
        table.startFunc()
    end
    return UI.UIInstance[table.name]
end

return UI
