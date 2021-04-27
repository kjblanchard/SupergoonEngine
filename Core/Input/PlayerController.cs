////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework.Input;

namespace SgEngine.Core.Input
{
    public class PlayerController : Controller
    {
        public int PlayerNum { get; }
        private KeyMapping<Keys> _keyMapping = DefaultKeyboardMap;
        private KeyMapping<Buttons> _buttonMapping = DefaultJoystickMap;
        public PlayerController(int playerNumber = 0)
        {
            PlayerNum = playerNumber;
        }

        public override bool IsButtonPressed(ControllerButtons button)
        {
            return button switch
            {
                ControllerButtons.Default => false,
                ControllerButtons.Up => _input.KeyPressed(_keyMapping.UpButton),
                ControllerButtons.Right => _input.KeyPressed(_keyMapping.RightButton),
                ControllerButtons.Down => _input.KeyPressed(_keyMapping.DownButton),
                ControllerButtons.Left => _input.KeyPressed(_keyMapping.LeftButton),
                ControllerButtons.Y => _input.KeyPressed(_keyMapping.YButton),
                ControllerButtons.B => _input.KeyPressed(_keyMapping.BButton),
                ControllerButtons.A => _input.KeyPressed(_keyMapping.AButton) || _input.KeyPressed(PlayerNum,_buttonMapping.AButton),
                ControllerButtons.X => _input.KeyPressed(_keyMapping.XButton),
                ControllerButtons.Start => _input.KeyPressed(_keyMapping.StartButton),
                ControllerButtons.Select => _input.KeyPressed(_keyMapping.SelectButton),
                _ => throw new ArgumentOutOfRangeException(nameof(button), button, null)
            };
        }

        public override bool IsButtonHeld(ControllerButtons button)
        {
            throw new System.NotImplementedException();
        }

        public override bool IsButtonReleased(ControllerButtons button)
        {
            throw new System.NotImplementedException();
        }
    }
}