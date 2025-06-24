-- UI.lua
local UI = {}
UI.UIInstance = {}
local font = ""
local fontSize = 0
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

local function CreatePanel(name, rect, parentPanel)
    rect = normalizeRect(rect)
    return cUI.CreatePanel(name, rect, parentPanel)
end

local function CreateImage(name, rect, parentPanel, filename, srcRect)
    rect = normalizeRect(rect)
    srcRect = normalizeRect(srcRect)
    return cUI.CreateImage(name, rect, parentPanel, filename, srcRect)
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

local function CreateButton(name, rect, parentPanel, pressedFunc, hoverFunc)
    rect = normalizeRect(rect)
    return cUI.CreateButton(name, rect, parentPanel, { pressedFunc, hoverFunc })
end

local function CreateUIObjectAndChildren(objTable, parentPtr, parentTable)
    -- TODO we should validate these so it doesn't break
    local node = { data = nil, children = {} }
    if objTable.type == "image" then
        node.data = CreateImage(objTable.name, objTable.location, parentPtr, objTable.imageName, objTable.srcRect)
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
    elseif objTable.type == "hlg" then
        node.data = CreateHLG(objTable.name, objTable.location, parentPtr, objTable.spacing)
    elseif objTable.type == "vlg" then
        node.data = CreateVLG(objTable.name, objTable.location, parentPtr, objTable.spacing)
    elseif objTable.type == "button" then
        node.data = CreateButton(objTable.name, objTable.location, parentPtr, objTable.pressedFunc, objTable.hoverFunc)
    elseif objTable.type == "panel" then
        node.data = CreatePanel(objTable.name, objTable.location, parentPtr)
    end
    if objTable.userdata ~= nil then node["userdata"] = objTable.userdata end
    parentTable[objTable.name] = node
    if objTable.children then
        for _, child in ipairs(objTable.children) do
            CreateUIObjectAndChildren(child, node.data, node.children)
        end
    end
    return node.data
end

-- TODO Only destroys top level panels currently
function UI.DestroyPanel(panelTable)
    cUI.DestroyUIObject(panelTable.data)
    for _, value in pairs(UI.UIInstance) do
        if value == panelTable then
            value = nil
        end
    end
    panelTable = nil
end

function UI.CreatePanelFromTable(table)
    -- Top level is always a panel
    local root = { data = CreatePanel(table.name, { 0, 0, 0, 0 }, nil), children = {}, doNotDestroy = table.doNotDestroy }
    UI.UIInstance[table.name] = root
    for _, child in ipairs(table.children) do
        CreateUIObjectAndChildren(child, root.data, root.children)
    end
    if table.startFunc ~= nil then
        table.startFunc()
    end
    return UI.UIInstance[table.name]
end

return UI
