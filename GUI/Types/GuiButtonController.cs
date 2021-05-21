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
        public bool ButtonsActive
        {
            get => _areButtonsActive;
            set
            {
                if (value && ButtonsToManage.Count >= CurrentSelection)
                    ButtonsToManage[CurrentSelection].IsSelected = true;
                _areButtonsActive = value;

            }
        }
        private bool _areButtonsActive = false;
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
        public GuiImageComponent cursorGuiImageComponent;
        public GuiButtonController(GuiComponent parent, Vector2 offset = new Vector2(), Point size = new Point()) : base(offset, size, parent)
        {

        }

        public void AddButton(GuiButton buttonToAdd)
        {
            ButtonsToManage.Add(buttonToAdd);
        }

        public void AddButtons(params GuiButton[] buttonsToAdd)
        {

            foreach (var button in buttonsToAdd)
            {

                ButtonsToManage.Add(button);
            }
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

        public void SelectButton(int newSelection)
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
            CheckForNewlyHoveredButtons();
            HandleMovingMouseOffSelection();
            HandleMouseSelectionWhenKeyboardMovedSelection();
            HandleLeftClick();

        }

        private void HandleMouseSelectionWhenKeyboardMovedSelection()
        {
            if (Controller.WasThereMouseMovement())
            {
                if (CurrentHoveredButtons.Count == 1)
                    SelectButton(CurrentHoveredButtons[0]);
            }
        }

        private void HandleMovingMouseOffSelection()
        {
            if (ButtonsToManage[CurrentSelection].WasJustLeftHovered)
            {
                if (CurrentHoveredButtons.Count > 0)
                    SelectButton(CurrentHoveredButtons[0]);
            }
        }

        private void CheckForNewlyHoveredButtons()
        {
            for (int i = 0; i < ButtonsToManage.Count; i++)
            {
                var currentButton = ButtonsToManage[i];
                if (currentButton._isHovered)
                    CurrentHoveredButtons.Add(i);
                if (ButtonsToManage[i].WasJustHovered)
                {
                    SelectButton(i);
                }
            }
        }

        private void HandleLeftClick()
        {
            if (!Controller.LeftMouseButtonClicked()) return;
            if (CurrentHoveredButtons.Contains(CurrentSelection))
            {
                ButtonsToManage[CurrentSelection].OnClick();
                return;
            }

            if (CurrentHoveredButtons.Count != 1) return;
            SelectButton(CurrentHoveredButtons[0]);
            ButtonsToManage[CurrentHoveredButtons[0]].OnClick();

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