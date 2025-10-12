local engine = require("Engine")
local TUI = {}
local CHILDTYPETEXT = 1

function TUI.NewPanel(objectPtr)
    return cTUI.NewPanel(objectPtr)
end

function TUI.NewPanelWithNameAndBorder(x, y, width, height, name)
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
    cTUI.UpdateText(textPtr, newText)
end

function TUI.AddLineTextWithColorToTextbox(textboxPtr, newText, color)
    -- Use white by default
    color = color or engine.Log.LogColorsWhite
    cTUI.AddLineTextWithColor(textboxPtr, newText, color)
end

function TUI.AddTextToTextbox(textboxPtr, newText)
    cTUI.AddText(textboxPtr, newText)
end

function TUI.AddColor(textboxPtr, color)
    color = color or engine.Log.LogColorsWhite
    cTUI.AddColor(textboxPtr, color)
end

function TUI.SetTextboxStyle(textboxPtr, styleInt)
    cTUI.AddStyle(textboxPtr, styleInt)
end

return TUI
