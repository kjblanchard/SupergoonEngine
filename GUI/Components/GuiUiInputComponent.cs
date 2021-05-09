////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using SgEngine.Core.Input;

namespace SgEngine.GUI.Components
{
    public class GuiUiInputComponent : GuiUiComponent
    {
        public GuiUiInputComponent() : base(new Vector2(), new Point())
        {
            
        }
        private Controller _currentController;

        public void TakeControl(Controller controller)
        {
            _currentController = controller;
        }

        public void RemoveControl()
        {
            _currentController = null;
        }

        public bool IsButtonJustPressed(ControllerButtons buttonToCheckFor)
        {
            return _currentController != null && _currentController.IsButtonPressed(buttonToCheckFor);
        }

        public bool IsButtonHeld(ControllerButtons buttonToCheckFor)
        {
            return _currentController != null && _currentController.IsButtonHeld(buttonToCheckFor);
        }

        public bool IsButtonJustReleased(ControllerButtons buttonToCheckFor)
        {
            return _currentController != null && _currentController.IsButtonReleased(buttonToCheckFor);
        }
    }
}