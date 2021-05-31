////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;
using Microsoft.Xna.Framework;
using SgEngine.GUI.Components;
using SgEngine.GUI.Types;
using SgEngine.Interfaces.SgDebug;
using SgEngine.Models.Ui;
using SgEngine.SgDebug;
using Panel = SgEngine.GUI.Types.Panel;

namespace SgEngine.SgJson.Parsing
{
    public abstract class UiParser : ISendDebugMessage
    {
        public event ISendDebugMessage.DebugLogEventHandler OnDebugMessage;

        protected UiParser()
        {
            OnDebugMessage += SgDebug.SgDebug.DebugMessage;
        }
        /// <summary>
        /// Loads from a file, and returns a Model for a UI screen
        /// </summary>
        /// <param name="fileToLoad">The actual file to load</param>
        /// <returns>Returns the deserialized json file</returns>
        public UiScreenModel LoadUiScreenJson(string fileToLoad)
        {

            var jsonReader = GameJsonLoader.CreateJsonStreamReader(fileToLoad, GameJsonLoader.ConfigFileToRead.UiScreenFile);
            var data = jsonReader.ReadToEnd();
            return JsonSerializer.Deserialize<UiScreenModel>(data);
        }

        /// <summary>
        /// Goes through the UIScreen and create all of the panels that are to be used
        /// </summary>
        /// <param name="loadedJsonScreen"></param>
        /// <returns></returns>
        public List<Panel> ParsePanelsFromJson(UiScreenModel loadedJsonScreen)
        {
            var panelList = new List<Panel>();
            foreach (var panelJson in loadedJsonScreen.Panels)
            {
                var panel = CreatePanel(panelJson);
                if (panelJson.TextComponents?.Count > 0)
                {
                    foreach (var textComponentJson in panelJson.TextComponents)
                    {
                        var guiTextComponent = CreateTextComponent(textComponentJson, panel);
                        panel.AddGuiComponent(guiTextComponent);
                    }
                }

                if (panelJson.Buttons?.Count > 0)
                {
                    foreach (var button in panelJson.Buttons)
                    {
                        var guiButton = CreateButton(button, panel);
                        AddButtonToPanel(guiButton, panel);
                    }

                }

                if (panelJson.ButtonControllers?.Count > 0)
                {
                    foreach (var buttonController in panelJson.ButtonControllers)
                    {
                        var guiButtonController = CreateButtonController(buttonController, panel);
                        foreach (var button in buttonController.Buttons)
                        {
                            var guiButton = CreateButton(button, panel);
                            guiButtonController.AddButton(guiButton);
                            guiButtonController.ButtonsActive = true;
                        }
                        panel.AddGuiComponent(guiButtonController);
                    }
                }
                panelList.Add(panel);
            }
            return panelList;
        }

        private void AddButtonToPanel(GuiButton guiButton, Panel panel)
        {
            if (guiButton != null)
                panel.AddGuiComponent(guiButton);
            else
            {
                SendDebugMessage("The button was null");
                Debug.WriteLine("The button was null");
            }
        }

        private Panel CreatePanel(Models.Ui.Panel panelJson)
        {
            var panelLocation = new Vector2(panelJson.Location.X, panelJson.Location.Y);
            var panelSize = new Point(panelJson.Size.X, panelJson.Size.Y);
            return panelJson.Graphic == -1 ? new Panel(panelLocation, panelSize) : new Panel(panelLocation, panelSize, panelJson.Graphic){ComponentName = panelJson.Name};
        }

        private GuiTextComponent CreateTextComponent(TextComponent textComponent, Panel parent)
        {
            var textConfig = PopulateTextConfig(textComponent, parent);
            return new GuiTextComponent(textConfig, textConfig.FontType);
        }

        protected TextBoxConfig PopulateTextConfig(TextComponent textComponentJson, GuiComponent parent)
        {
            var textConfig = new TextBoxConfig();
            textConfig.Alignment = textComponentJson.Alignment switch
            {
                "Center" => GuiTextComponent.Alignment.Center,
                "Left" => GuiTextComponent.Alignment.Left,
                _ => textConfig.Alignment
            };
            textConfig.DisplayText = textComponentJson.DisplayText;
            textConfig.FontType = textComponentJson.FontType;
            textConfig.Parent = parent;
            textConfig.ParentOffset = new Vector2(textComponentJson.ParentOffset.X, textComponentJson.ParentOffset.Y);
            return textConfig;
        }

        /// <summary>
        /// To load guibuttons, this needs to be overridden
        /// </summary>
        /// <param name="buttonModel"></param>
        /// <param name="parent"></param>
        /// <returns></returns>
        public abstract GuiButton CreateButton(Button buttonModel, GuiComponent parent);

        /// <summary>
        /// To load Button controllers, this needs to be overridden
        /// </summary>
        /// <param name="buttonControllerJson"></param>
        /// <param name="parent"></param>
        /// <returns></returns>
        public abstract GuiButtonController CreateButtonController(ButtonController buttonControllerJson, GuiComponent parent);
        public void SendDebugMessage(string messageToWrite, LogLevel logLevel = LogLevel.Debug)
        {
            OnDebugMessage?.Invoke(this,messageToWrite,logLevel);
        }
    }
}