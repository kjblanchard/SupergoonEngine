﻿////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core.Input;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{

    public abstract class GuiButtonController : GuiComponent
    {
        /// <summary>
        /// The cursor for this button controller, can be assigned in the parent class.  Left this here so that when buttons become active it shows the cursor
        /// </summary>
        public GuiImageComponent CursorGuiImageComponent;
        /// <summary>
        /// Selects the current selection and sets the buttons active to true.  Probably allows movement in the icons too
        /// </summary>
        public bool ButtonsActive
        {
            get => _areButtonsActive;
            set
            {
                if (value && ButtonsToManage.Count >= CurrentSelection)
                    ButtonsToManage[CurrentSelection].IsSelected = true;
                if (CursorGuiImageComponent != null)
                    CursorGuiImageComponent.IsVisible = true;
                _areButtonsActive = value;
            }
        }
        private bool _areButtonsActive;
        /// <summary>
        /// Returns the current selection, also will loop to the other side of the buttons if should loop selection is true
        /// </summary>
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
            }
        }
        private int _currentSelectedButton;
        public List<GuiButton> ButtonsToManage = new List<GuiButton>();
        public List<int> CurrentHoveredButtons = new List<int>();
        private bool _shouldLoopSelection = true;

        protected GuiButtonController(GuiComponent parent, Vector2 offset = new Vector2(), Point size = new Point()) : base(offset, size, parent)
        {

        }



        public void AddButton(GuiButton buttonToAdd)
        {
            ButtonsToManage.Add(buttonToAdd);
            buttonToAdd.Initialize();
            buttonToAdd.LoadContent();
            buttonToAdd.BeginRun();
        }

        public void AddButton(params GuiButton[] buttonsToAdd)
        {
            foreach (var button in buttonsToAdd)
            {
                AddButton(button);
            }
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            if (_areButtonsActive)
                HandleMouse();
            CursorGuiImageComponent?.Update(gameTime);
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.Draw(gameTime, spriteBatch);
            }
            CursorGuiImageComponent?.Draw(gameTime, spriteBatch);
        }

        public override void HandleMouse()
        {
            base.HandleMouse();
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.HandleMouse();
            }
            HandleMouseInput();
        }


        /// <summary>
        /// Handles all of the mouse functions that happens this frame on the button controller
        /// </summary>
        protected void HandleMouseInput()
        {
            CurrentHoveredButtons.Clear();
            SelectNewlyHoveredButton();
            MouseMovingOffCurrentSelection();
            UpdateSelectionOnMouseMovement();
            HandleLeftClick();
        }
        /// <summary>
        /// If you just hover over a button, select that button
        /// </summary>
        private void SelectNewlyHoveredButton()
        {
            for (int i = 0; i < ButtonsToManage.Count; i++)
            {
                var currentButton = ButtonsToManage[i];
                if (currentButton.IsHovered)
                    CurrentHoveredButtons.Add(i);
                if (ButtonsToManage[i].WasJustHovered)
                {
                    SelectButton(i, true);
                }
            }
        }
        /// <summary>
        /// Handles if you are hovering over two options, and you move off of the current selection with your mouse to jump to the other hovered item
        /// </summary>
        private void MouseMovingOffCurrentSelection()
        {
            if (ButtonsToManage[CurrentSelection].WasJustLeftHovered)
            {
                if (CurrentHoveredButtons.Count > 0)
                    SelectButton(CurrentHoveredButtons[0], true);
            }
        }
        /// <summary>
        /// If there was mouse movement, select the button you are hovered over.  Handles if there was keyboard/joystick movement and then mouse movement after
        /// </summary>
        private void UpdateSelectionOnMouseMovement()
        {
            if (Controller.WasThereMouseMovement())
            {
                if (CurrentHoveredButtons.Count == 1)
                    SelectButton(CurrentHoveredButtons[0], true);
            }
        }
        /// <summary>
        /// Handles the left mouse button clicks
        /// </summary>
        private void HandleLeftClick()
        {
            if (!Controller.LeftMouseButtonClicked()) return;
            if (ClickIfButtonIsHovered()) return;
            SelectAndClickIfPossible();
        }
        /// <summary>
        /// If you are hovering over a button, click it, otherwise don't
        /// </summary>
        /// <returns>True if the button was clicked</returns>
        private bool ClickIfButtonIsHovered()
        {
            if (!CurrentHoveredButtons.Contains(CurrentSelection)) return false;
            PressButton(CurrentSelection, true);
            return true;

        }
        /// <summary>
        /// If you are hovering over one button, select it and then click on it.  If not, return
        /// </summary>
        private void SelectAndClickIfPossible()
        {
            if (CurrentHoveredButtons.Count != 1) return;
            SelectButton(CurrentHoveredButtons[0], true);
            PressButton(CurrentSelection, true);
        }
        protected virtual void SelectButton(int newSelection, bool selectedByMouse = false)
        {
            if (CurrentSelection == newSelection)
                return;
            ButtonsToManage[CurrentSelection].IsSelected = false;
            CurrentSelection = newSelection;
            ButtonsToManage[CurrentSelection].IsSelected = true;
        }

        protected virtual void PressButton(int buttonToPress, bool pressedByMouse = false)
        {
            ButtonsToManage[CurrentSelection].OnClick();
        }
        /// <summary>
        /// Turns on debug mode for all of the buttons
        /// </summary>
        public void AllButtonDebugMode()
        {
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.DebugMode = true;
            }
        }


    }
}