-- UI.lua
local UI = {}
local engine = require("Engine")
UI.UIInstance = {}
UI.lookup = {}
local font = ""
local fontSize = 0

---@class UIObject
---@field data lightuserdata     -- Ptr to the C uiobject pointer
---@field userdata lightuserdata     -- Ptr to the C uiobject pointer
---@field visible boolean     -- Ptr to the C uiobject pointer
---@field key string     -- Ptr to the C uiobject pointer
---@field doNotDestroy   boolean -- If this object is set to be donotdestroy, which affects all children as well.
---@field children   table{UIObject} -- Children of this uiobject

local function makeKey(parentKey, name)
    if parentKey == nil or parentKey == "" then
        return name
    else
        return parentKey .. "." .. name
    end
end
local function setupUIObject(uiObjectPtr, tableData, parentTable)
    local doNotDestroy = tableData.doNotDestroy or false
    local parentKey = (parentTable and parentTable.key) or ""
    local key = makeKey(parentKey, tableData.name)
    local objectTable = { data = uiObjectPtr, children = {}, doNotDestroy = doNotDestroy, key = key }
    if parentTable then parentTable[tableData.name] = objectTable end
    if tableData.visible ~= nil and tableData.visible ~= true then UI.SetObjectVisible(objectTable.data, false) end
    UI.lookup[key] = objectTable
    return objectTable
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

local function Create9SliceImage(name, rect, parentPanel, filename, color, xsize, ysize)
    local x = xsize or 8
    local y = ysize or 9
    rect = normalizeRect(rect)
    color = normalizeColorRect(color)
    return cUI.Create9SliceImage(name, rect, parentPanel, filename, color, x, y)
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

---The Thing, make it
---@param objTable table
---@param parentTable UIObject parent ui object
---@return unknown|nil
local function CreateUIObjectAndChildren(objTable, parentTable)
    if objTable.isMobile and not engine.IsMobile() then return end
    if not objTable.name then
        engine.Log.LogWarn("No table name for this table, making name be parent ptr")
        objTable.name = tostring(parentTable.data)
    end
    local ptr = nil

    if objTable.type == "image" then
        ptr = CreateImage(objTable.name, objTable.location, parentTable.data, objTable.imageName, objTable.srcRect,
            objTable.transparency)
    elseif objTable.type == "imageAnimator" then
        ptr = CreateImageAnimator(objTable.name, objTable.location, parentTable.data, objTable.imageName, objTable
            .srcRect, objTable.transparency, objTable.defaultAnim)
    elseif objTable.type == "9slice" then
        ptr = Create9SliceImage(objTable.name, objTable.location, parentTable.data, objTable.imageName,
            objTable.color)
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
        ptr = CreateText(objTable.name, objTable.location, parentTable.data, objTable.text, thisFont, thisSize,
            objTable.centeredX, objTable.centeredY, objTable.wordWrap, objTable.color)
    elseif objTable.type == "rect" then
        ptr = CreateRect(objTable.name, objTable.location, parentTable.data, objTable.color)
    elseif objTable.type == "progressBar" then
        ptr = CreateProgressBar(objTable.name, objTable.location, parentTable.data, objTable.color)
    elseif objTable.type == "hlg" then
        ptr = CreateHLG(objTable.name, objTable.location, parentTable.data, objTable.spacing)
    elseif objTable.type == "vlg" then
        ptr = CreateVLG(objTable.name, objTable.location, parentTable.data, objTable.spacing)
    elseif objTable.type == "button" then
        ptr = CreateButton(objTable.name, objTable.location, parentTable.data, objTable.pressedFunc,
            objTable.hoverFunc,
            objTable.pressOnRelease)
    elseif objTable.type == "panel" then
        ptr = CreatePanel(objTable.name, objTable.location, parentTable.data)
    end
    local node = setupUIObject(ptr, objTable, parentTable)

    if objTable.children then
        for _, child in ipairs(objTable.children) do
            local newChild = CreateUIObjectAndChildren(child, node)
        end
    end
    return ptr
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
function UI.PlayAnimation(animData, animName, loops)
    loops = loops or -1
    cUI.PlayAnimation(animData, animName, loops)
end

function UI.UpdateNumLettersForText(textData, numLetters)
    cUI.TextSetNumLetters(textData, numLetters)
end

function UI.CreateUIFromUIFile(table)
    if table.isMobile and not engine.IsMobile() then return end
    if UI.UIInstance[table.name] ~= nil then
        engine.Log.LogDebug(string.format("Trying to create a panel that already exists in root, not creating %s !",
            table.name))
        return
    end
    local ptr = CreatePanel(table.name, { 0, 0, 0, 0 }, nil)
    local newObjPtrTable = setupUIObject(ptr, table, nil)
    UI.UIInstance[table.name] = newObjPtrTable
    for _, childData in ipairs(table.children) do
        -- CreateUIObjectAndChildren(childData, newObjPtrTable.data, newObjPtrTable.children, "")
        CreateUIObjectAndChildren(childData, newObjPtrTable)
    end
    if table.startFunc ~= nil then
        table.startFunc()
    end
    return UI.UIInstance[table.name]
end

return UI
