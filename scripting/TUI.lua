local engine = require("Engine")
local TUI = {}
local CHILDTYPETEXT = 1

function TUI.NewPanel(objectPtr)
    return cTUI.NewPanel(objectPtr)
end

function TUI.NewPanelWithNameAndBorder(width, height, x, y, name)
    return cTUI.NewPanelEx(width, height, x, y, true, name)
end

function TUI.DrawPanel(panel)
    return cTUI.DrawPanel(panel)
end

function TUI.NewText(parentPanel, offsetX, offsetY, text)
    return cTUI.NewText(parentPanel, offsetX, offsetY, text)
end

function TUI.AddTextToPanel(panel, text)
    return cTUI.AddChild(panel, text, CHILDTYPETEXT)
end

function TUI.UpdateText(textPtr, newText)
    return cTUI.UpdateText(textPtr, newText)
end

return TUI
