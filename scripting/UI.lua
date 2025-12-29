local UI = {}
UI.UITree = {}
local engine = require("Engine")
-- Current font and size stays cached, so we don't have to type font in every time, only the first time we make a text
local currentFont = ""
local currentFontSize = nil
-- used to cache the draw image rect
local drawImageRect = { x = 0, y = 0, w = 0, h = 0 }

local function createImageFromTable(name, dataTable, objTable)
    objTable.texture = engine.CreateTexture(dataTable.filename)
    dataTable.srcRect = engine.Tools.NormalizeRect(dataTable.srcRect)
    objTable.rect = engine.Tools.NormalizeRect(dataTable.rect)
end

local function drawImage(parentOffsetX, parentOffsetY, uiObject)
    if uiObject and uiObject.texture then
        -- drawImageRect.x = imageTable.rect.x + parentOffsetX
        -- drawImageRect.y = imageTable.rect.y + parentOffsetY
        drawImageRect.x = uiObject.rect.x
        drawImageRect.y = uiObject.rect.y
        drawImageRect.w = uiObject.rect.w
        drawImageRect.h = uiObject.rect.h
        engine.DrawTexture(uiObject.texture, engine.GetSpriteShader(), drawImageRect, uiObject.dataTable.srcRect)
    end
    if uiObject.dataTable.d then
        engine.DrawRect(drawImageRect, false, false)
    end
end

local function createTextFromTable(name, dataTable, objTable)
    local font = dataTable.font or currentFont
    local fontSize = dataTable.fontSize or currentFontSize
    if not font or not fontSize or not dataTable.rect or not dataTable.text then
        engine.Log.LogWarn(string.format("Bad table passed in for text for %s table", name))
    end
    currentFont = font
    currentFontSize = fontSize
    local numChars = dataTable.chars or #dataTable.text
    local x = dataTable.centerX ~= false
    local y = dataTable.centerY ~= false
    objTable.ptr = engine.Text.CreateText(font, fontSize, objTable.rect, dataTable.text, numChars, x, y)
end

local function drawText(parentOffsetX, parentOffsetY, uiObject)
    if uiObject and uiObject.ptr then
        engine.Text.DrawText(uiObject.ptr, parentOffsetX, parentOffsetY)
    end

    if uiObject.dataTable.d then
        local test = {
            x = uiObject.rect.x,
            y = uiObject.rect.y,
            w = uiObject.rect.w,
            h = uiObject.rect.h,
        }
        test.x = test.x + parentOffsetX
        test.y = test.y + parentOffsetY
        engine.DrawRect(test, false, false)
    end
end

local function destroyText(uiObject)
    if uiObject then cGraphics.DestroyText(uiObject.ptr) end
end

local function create9SliceFromTable(name, dataTable, objTable)
    dataTable.color = engine.Tools.NormalizeArrayTableWithKeys(dataTable.color, { "r", "g", "b", "a" })
    objTable.ptr = cGraphics.Create9SliceTexture(
        objTable.rect,
        dataTable.filename,
        dataTable.color,
        dataTable.xSize,
        dataTable.ySize
    )
end

local function draw9Slice(parentOffsetX, parentOffsetY, uiObject)
    if uiObject and uiObject.ptr then
        local drawRect = { uiObject.rect.x + parentOffsetX, uiObject.rect.y + parentOffsetY, uiObject
            .rect.w, uiObject.rect.h }
        drawRect = engine.Tools.NormalizeRect(drawRect)
        cGraphics.DrawNineSlice(uiObject.ptr, drawRect)
        if uiObject.dataTable.d then
            engine.DrawRect(drawRect, false, false)
        end
    end
end
local function destroy9Slice(uiObject)
    if uiObject then cGraphics.DestroyTexture(uiObject.ptr) end
end

-- Function that takes in name, dataTable (from load file or obj) and the new object
local classTypeFunctionTable = {
    text = createTextFromTable,
    nineSlice = create9SliceFromTable,
    image = createImageFromTable
}

local classTypeDrawTable = {
    text = drawText,
    nineSlice = draw9Slice,
    image = drawImage
}

local classTypeDestroyTable = {
    nineSlice = destroy9Slice,
    text = destroyText
}

function UI.UpdateTextText(textPtr, newText)
    cText.UpdateTextText(textPtr, newText)
end

function LoadUIObjectFromTable(parentObj, dataName, uiObject)
    local newChildTable = {
        dataTable = uiObject,
        visible = (uiObject.visible == nil) and true or uiObject.visible,
        children = {},
        rect = { table.unpack(uiObject.rect) },
        parent = parentObj,
        ptr = 0
    }
    newChildTable.rect = engine.Tools.NormalizeRect(uiObject.rect)
    if uiObject.class and classTypeFunctionTable[uiObject.class] then
        classTypeFunctionTable[uiObject.class](dataName, uiObject, newChildTable)
    end
    if uiObject.children then
        for childName, childValue in pairs(uiObject.children) do
            LoadUIObjectFromTable(newChildTable, childName, childValue)
        end
    end
    parentObj.children[dataName] = newChildTable
end

function UI.CreateUIPanelFromScriptFile(file)
    if UI.UITree[file] then return end
    local dataTable = require(file)
    local newParentPanel = {
        dataTable = dataTable,
        visible = (dataTable.visible == nil) and true or dataTable.visible,
        children = {},
        rect = engine.Tools.NormalizeRect({ 0, 0, 0, 0 }),
        parent = nil,
        ptr = 0,
    }
    for childDataName, childDataTable in pairs(newParentPanel.dataTable.children) do
        LoadUIObjectFromTable(newParentPanel, childDataName, childDataTable)
    end
    UI.UITree[file] = newParentPanel
    if newParentPanel.dataTable.startFunc then
        newParentPanel.dataTable.startFunc()
    end
end

local function drawUIRecursive(parentX, parentY, uiObject)
    if not uiObject.visible then return end
    if classTypeDrawTable[uiObject.dataTable.class] then
        classTypeDrawTable[uiObject.dataTable.class](parentX, parentY, uiObject)
    end
    --TODO Make a priority table, currently  if we add a child this is not remade
    if uiObject.children and uiObject.childrenPriorityDraw == nil then
        local sortedChildren = {}
        for _, child in pairs(uiObject.children) do
            table.insert(sortedChildren, child)
        end
        table.sort(sortedChildren, function(a, b)
            return (a.dataTable.priority or 0) < (b.dataTable.priority or 0)
        end)
        uiObject.childrenPriorityDraw = sortedChildren
        uiObject.childrenPriorityDraw = sortedChildren
    end
    for i = 1, #uiObject.childrenPriorityDraw, 1 do
        local childTable = uiObject.childrenPriorityDraw[i]
        drawUIRecursive(parentX + uiObject.rect.x, parentY + uiObject.rect.y, childTable)
    end
end

local function destroyUIRecursive(uiObject)
    if classTypeDestroyTable[uiObject.dataTable.class] then
        classTypeDestroyTable[uiObject.dataTable.class]()
    end
    for _, childTable in pairs(uiObject.children) do
        destroyUIRecursive(childTable)
    end
end

function UI.DrawUI()
    for _, panelChild in pairs(UI.UITree) do
        drawUIRecursive(0, 0, panelChild)
    end
end

function UI.DestroyUI()
    for _, panelChild in pairs(UI.UITree) do
        destroyUIRecursive(panelChild)
    end
end

function UI.GetWorldXYForObject(uiObject)
    local x = uiObject.rect.x
    local y = uiObject.rect.y
    local parent = uiObject.parent
    while parent do
        engine.Log.LogWarn("Current x " .. x .. " parent x " .. parent.rect.x)
        x = x + parent.rect.x
        y = y + parent.rect.y
        parent = parent.parent
    end
    return x, y
end

return UI
