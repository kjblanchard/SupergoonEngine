////////////////////////////////////////////////////////////
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
using SgEngine.EKS;
using SgEngine.GUI.Components;
using SgEngine.Interfaces;
using SgEngine.Interfaces.Sound;

namespace SgEngine.GUI.Types
{

    public abstract class GuiButtonController : GuiComponent, IPlaySfx
    {
        protected IPlaySfx AsIPlaySfx => (IPlaySfx)this;
        /// <summary>
        /// The cursor for this button controller, can be assigned in the parent class
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

        protected Enum _moveSoundEffect;
        protected Enum _selectSoundEffect;
        protected Enum _cancelSoundEffect;

        private int _currentSelectedButton;
        private bool _shouldLoopSelection = true;
        public List<GuiButton> ButtonsToManage = new List<GuiButton>();
        public List<int> CurrentHoveredButtons = new List<int>();
        private PlayerController _playerController = GameWorld.GetPlayerController(0);

        protected GuiButtonController(GuiComponent parent, Vector2 offset = new Vector2(), Point size = new Point()) : base(offset, size, parent)
        {

        }


        public override void Initialize()
        {
            base.Initialize();
            CursorGuiImageComponent?.Initialize();
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
            if (_areButtonsActive && _playerController != null)
                HandleInput();
            CursorGuiImageComponent?.Update(gameTime);
        }

        public void TakeControl(PlayerController controllerToControl)
        {
            _playerController = controllerToControl;
        }

        public void RemoveControl()
        {
            _playerController = null;
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

        protected void HandleKeyboardInput()
        {
            if (_playerController.IsButtonPressed(ControllerButtons.Down))
                SelectButton(CurrentSelection + 1);
            else if (_playerController.IsButtonPressed(ControllerButtons.Up))
                SelectButton(CurrentSelection - 1);
            if (_playerController.IsButtonPressed(ControllerButtons.A))
                Pressbutton(CurrentSelection);
        }

        protected void SelectButton(int newSelection, bool selectedByMouse = false)
        {
            if (CurrentSelection == newSelection)
                return;
            ButtonsToManage[CurrentSelection].IsSelected = false;
            CurrentSelection = newSelection;
            ButtonsToManage[CurrentSelection].IsSelected = true;
            if (!selectedByMouse && _moveSoundEffect != null)
                AsIPlaySfx.PlaySfx(_moveSoundEffect);
        }

        protected void Pressbutton(int buttonToPress, bool pressedByMouse = false)
        {
            ButtonsToManage[CurrentSelection].OnClick();
            if (_selectSoundEffect != null)
                AsIPlaySfx.PlaySfx(_selectSoundEffect);
        }

        protected void HandleMouseInput()
        {
            CurrentHoveredButtons.Clear();
            SelectNewlyHoveredButton();
            MouseMovingOffCurrentSelection();
            UpdateSelectionOnMouseMovement();
            HandleLeftClick();
        }
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
        private void MouseMovingOffCurrentSelection()
        {
            if (ButtonsToManage[CurrentSelection].WasJustLeftHovered)
            {
                if (CurrentHoveredButtons.Count > 0)
                    SelectButton(CurrentHoveredButtons[0], true);
            }
        }

        private void UpdateSelectionOnMouseMovement()
        {
            if (Controller.WasThereMouseMovement())
            {
                if (CurrentHoveredButtons.Count == 1)
                    SelectButton(CurrentHoveredButtons[0], true);
            }
        }
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
            Pressbutton(CurrentSelection,true);
            return true;

        }
        /// <summary>
        /// If you are hovering over one button, select it and then click on it.  If not, return
        /// </summary>
        private void SelectAndClickIfPossible()
        {
            if (CurrentHoveredButtons.Count != 1) return;
            SelectButton(CurrentHoveredButtons[0],true);
            Pressbutton(CurrentSelection, true);
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