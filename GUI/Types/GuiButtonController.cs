////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core.Input;
using SgEngine.EKS;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{
    public class GuiButtonController : GuiComponent
    {
        private bool _shouldLoopSelection = true;
        public int CurrentSelection
        {
            get => _currentSelectedButton;
            set
            {
                var newValue = value;
                switch (_shouldLoopSelection)
                {
                    case true:
                        if (value > ButtonsToManage.Count - 1)
                            newValue = 0;
                        if (value < 0)
                            newValue = ButtonsToManage.Count - 1;
                        _currentSelectedButton = newValue;
                        break;
                    case false:
                        _currentSelectedButton = Math.Clamp(value, 0, ButtonsToManage.Count - 1);
                        break;
                }
                Debug.WriteLine("Current selection just switched to " + _currentSelectedButton);
            }
        }

        private int _currentSelectedButton;
        public List<GuiButton> ButtonsToManage = new List<GuiButton>();
        public List<int> CurrentHoveredButtons = new List<int>();
        public PlayerController playerController = GameWorld.GetPlayerController(0);
        public GuiButtonController(GuiComponent parent, Vector2 offset = new Vector2(), Point size = new Point()) : base(offset, size, parent)
        {
        }

        public void AddButton(GuiButton buttonToAdd)
        {
            ButtonsToManage.Add(buttonToAdd);
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            HandleInput();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.Draw(gameTime, spriteBatch);
            }
        }

        public override void HandleInput()
        {
            base.HandleInput();
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.HandleInput();
            }
            HandleKeyboardInput();
            HandleMouseInput();
        }

        private void HandleKeyboardInput()
        {
            if (playerController.IsButtonPressed(ControllerButtons.Down))
                SelectButton(CurrentSelection + 1);
            else if (playerController.IsButtonPressed(ControllerButtons.Up))
                SelectButton(CurrentSelection - 1);
            if (playerController.IsButtonPressed(ControllerButtons.A))
                ButtonsToManage[CurrentSelection].OnClick();
        }

        private void SelectButton(int newSelection)
        {
            if (CurrentSelection == newSelection)
                return;
            ButtonsToManage[CurrentSelection].IsSelected = false;
            CurrentSelection = newSelection;
            ButtonsToManage[CurrentSelection].IsSelected = true;
        }

        private void HandleMouseInput()
        {
            CurrentHoveredButtons.Clear();
            //Check for a newly hovered button, select it if there is
            for (int i = 0; i < ButtonsToManage.Count; i++)
            {
                var currentButton = ButtonsToManage[i];
                if(currentButton._isHovered)
                    CurrentHoveredButtons.Add(i);
                if (ButtonsToManage[i].WasJustHovered)
                {
                    SelectButton(i);
                }
            }

            //Handle if you just moved off the button with your mouse
            if (ButtonsToManage[CurrentSelection].WasJustLeftHovered)
            {
                for (int i = 0; i < ButtonsToManage.Count; i++)
                {
                    if (ButtonsToManage[i]._isHovered)
                    {
                        Debug.WriteLine("This button was just chosen cause someone left his hover over this frame " + ButtonsToManage[i]._guiTextComponent.DisplayText);
                        SelectButton(i);
                    }
                }

            }

        }

        public void AllButtonDebugMode()
        {
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.DebugMode = true;
            }
        }

    }
}