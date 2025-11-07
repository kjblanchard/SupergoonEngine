local UI = {}
UI.UITree = {}
local engine = require("Engine")
-- Current font and size stays cached, so we don't have to type font in every time, only the first time we make a text
local currentFont = ""
local currentFontSize = nil

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
    objTable.ptr = engine.Text.CreateText(font, fontSize, objTable.rect,dataTable.text, numChars,x, y )

end

local function drawText(parentOffsetX, parentOffsetY, textDataTable)
    if textDataTable and textDataTable.ptr then
        engine.Text.DrawText(textDataTable.ptr, parentOffsetX, parentOffsetY)
    end

    if textDataTable.dataTable.d then
        engine.StartDebugger()
        local test = {
            x = textDataTable.rect.x,
            y = textDataTable.rect.y,
            w = textDataTable.rect.w,
            h = textDataTable.rect.h,
        }
        test.x = test.x + parentOffsetX
        test.y = test.y + parentOffsetY
        engine.DrawRect(test, false)

    end
end

local function create9SliceFromTable(name, dataTable, objTable)
    dataTable.color = engine.Tools.NormalizeArrayTableWithKeys(dataTable.color, {"r", "g", "b", "a"})
    objTable.ptr = cGraphics.Create9SliceTexture(
        objTable.rect,
        dataTable.filename,
        dataTable.color,
        dataTable.xSize,
        dataTable.ySize
        )
end

local function draw9Slice(parentOffsetX, parentOffsetY, textDataTable)
    if textDataTable and textDataTable.ptr then
        local drawRect = {textDataTable.rect.x + parentOffsetX, textDataTable.rect.y + parentOffsetY, textDataTable.rect.w, textDataTable.rect.h}
        drawRect = engine.Tools.NormalizeRect(drawRect)
        cGraphics.DrawNineSlice(textDataTable.ptr, drawRect)
    end
end

-- Function that takes in name, dataTable (from load file or obj) and the new object
local classTypeFunctionTable = {
    text = createTextFromTable,
    nineSlice = create9SliceFromTable
}

local classTypeDrawTable = {
    text = drawText,
    nineSlice = draw9Slice
}

function UI.UpdateTextText(textPtr, newText)
    cText.UpdateTextText(textPtr, newText)
end


function LoadUIObjectFromTable(parentObj, dataName, dataTable)
    local newChildTable = {
        dataTable = dataTable,
        visible = (dataTable.visible == nil) and true or dataTable.visible,
        children = {},
        rect = {table.unpack(dataTable.rect)},
        parent = parentObj,
        ptr = 0
    }
    newChildTable.rect = engine.Tools.NormalizeRect(dataTable.rect)
    if dataTable.class and classTypeFunctionTable[dataTable.class] then
        classTypeFunctionTable[dataTable.class](dataName, dataTable, newChildTable)
    end
    if dataTable.children then
        for childName, childValue in pairs(dataTable.children) do
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
        rect = engine.Tools.NormalizeRect({0,0,0,0}),
        parent = 0,
        ptr = 0,
    }
    for childDataName, childDataTable in pairs(newParentPanel.dataTable.children) do
        engine.Log.LogWarn("Creating child")
        LoadUIObjectFromTable(newParentPanel, childDataName, childDataTable)
    end
    UI.UITree[file] = newParentPanel
    if newParentPanel.dataTable.startFunc then
        newParentPanel.dataTable.startFunc()
    end
end

local function drawUIRecursive(parentX, parentY, tableData)
    if not tableData.visible then return end
    if  classTypeDrawTable[tableData.dataTable.class] then
        classTypeDrawTable[tableData.dataTable.class](parentX, parentY, tableData)
    end
    for _, childTable in pairs(tableData.children) do
        drawUIRecursive(parentX + tableData.rect.x, parentY + tableData.rect.y,childTable )
    end
end

function UI.DrawUI()
    for _, panelChild in pairs(UI.UITree) do
        drawUIRecursive(0, 0, panelChild)
    end
end

return UI
