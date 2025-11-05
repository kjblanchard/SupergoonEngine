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
    objTable.ptr = engine.Text.CreateText(font, fontSize, dataTable.rect,dataTable.text)
    currentFont = font
    currentFontSize = fontSize
end

local function drawText(parentOffsetX, parentOffsetY, textDataTable)
    if textDataTable and textDataTable.ptr then
        engine.Text.DrawText(textDataTable.ptr, parentOffsetX, parentOffsetY)
    end
end

-- Function that takes in name, dataTable (from load file or obj) and the new object
local classTypeFunctionTable = {
    text = createTextFromTable
}

local classTypeDrawTable = {
    text = drawText
}


function LoadUIObjectFromTable(parentObj, dataName, dataTable)
    local newChildTable = {
        dataTable = dataTable,
        children = {},
        rect = {},
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
    local newParentPanel = {
        dataTable = require(file),
        children = {},
        rect = engine.Tools.NormalizeRect({0,0,0,0}),
        parent = 0,
        ptr = 0
    }
    for childDataName, childDataTable in pairs(newParentPanel.dataTable.children) do
        LoadUIObjectFromTable(newParentPanel, childDataName, childDataTable)
    end
    UI.UITree[file] = newParentPanel
end

local function drawUIRecursive(parentX, parentY, tableData)
    engine.StartDebugger()
    if classTypeDrawTable[tableData.dataTable.class] then
        classTypeDrawTable[tableData.dataTable.class](parentX, parentY, tableData)
    end
    for _, childTable in pairs(tableData.children) do
        drawUIRecursive(parentX + tableData.rect.x, parentY + tableData.rect.y,childTable )
    end
end

function UI.DrawUI()
    -- for each UI panel page
    for _, panelChild in pairs(UI.UITree) do
        drawUIRecursive(0, 0, panelChild)
    end
end

return UI
