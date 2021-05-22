////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Text.Json;
using FMOD;
using Microsoft.Xna.Framework;
using SgEngine.Core;
using SgEngine.GUI.Components;
using SgEngine.Models.Ui;
using Panel = SgEngine.GUI.Panel;

namespace SgEngine.SgJson.Parsing
{
    public class UiParser
    {
        public static UiScreenModel LoadUiScreenJson(string fileToLoad)
        {

            var jsonReader = GameJsonLoader.CreateJsonStreamReader(fileToLoad, GameJsonLoader.ConfigFileToRead.UiScreenFile);
            var data = jsonReader.ReadToEnd();
            return JsonSerializer.Deserialize<UiScreenModel>(data);
        }

        public static List<Panel> ParsePanelsFromJson(UiScreenModel loadedJsonScreen)
        {
            var panelList = new List<Panel>();
            foreach (var panelJson in loadedJsonScreen.Panels)
            {
                var panel = CreatePanel(panelJson);
                if (panelJson.TextComponents.Count > 0)
                {
                    foreach (var textComponentJson in panelJson.TextComponents)
                    {
                        var guiTextComponent = CreateTextComponent(textComponentJson, panel);
                        panel.AddUiObject(guiTextComponent);
                    }
                }
                panelList.Add(panel);
            }
            return panelList;
        }

        private static Panel CreatePanel(Models.Ui.Panel panelJson)
        {
            var panelLocation = new Vector2(panelJson.Location.X, panelJson.Location.Y);
            var panelSize = new Point(panelJson.Size.X, panelJson.Size.Y);
            return panelJson.Graphic == -1 ? new Panel(panelLocation, panelSize) : new Panel(panelLocation, panelSize, panelJson.Graphic);
        }

        private static GuiTextComponent CreateTextComponent(Models.Ui.TextComponent textComponent, Panel parent)
        {

            var textConfig = new TextBoxConfig();
            textConfig.Alignment = textComponent.Alignment switch
            {
                "Center" => GuiTextComponent.Alignment.Center,
                "Left" => GuiTextComponent.Alignment.Left,
                _ => textConfig.Alignment
            };
            textConfig.DisplayText = textComponent.DisplayText;
            textConfig.FontType = textComponent.FontType;
            textConfig.Parent = parent;
            textConfig.ParentOffset =
                new Vector2(textComponent.ParentOffset.X, textComponent.ParentOffset.Y);

            return new GuiTextComponent(textConfig, textConfig.FontType);
        }
    }
}