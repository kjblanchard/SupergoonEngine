﻿////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;

namespace SgEngine.Core.Input
{
    /// <summary>
    /// Base class used for AI controllers and for player controllers
    /// </summary>
    public abstract class Controller
    {
        protected Controller()
        {
            _input ??= GameWorld.Input;
        }
        /// <summary>
        /// Default keyboard mapping for player 1
        /// </summary>
        protected static KeyMapping<Keys> _defaultKeyboardMap = new KeyMapping<Keys>()
        {
            UpButton = Keys.W,
            RightButton = Keys.D,
            DownButton = Keys.S,
            LeftButton = Keys.A,
            YButton = Keys.Y,
            BButton = Keys.X,
            AButton = Keys.Space,
            XButton = Keys.P,
            StartButton = Keys.Enter,
            SelectButton = Keys.Apps
        };

        /// <summary>
        /// The default button mapping in the game
        /// </summary>
        protected static KeyMapping<Buttons> _defaultJoystickMap = new KeyMapping<Buttons>
        {
            UpButton = Buttons.DPadUp,
            RightButton = Buttons.DPadRight,
            DownButton = Buttons.DPadDown,
            LeftButton = Buttons.DPadLeft,
            YButton = Buttons.Y,
            BButton = Buttons.B,
            AButton = Buttons.A,
            XButton = Buttons.X,
            StartButton = Buttons.Start,
            SelectButton = Buttons.Back
        };

        /// <summary>
        /// The input to check if buttons are pressed
        /// </summary>
        protected static InputSg _input;

        public abstract bool IsButtonPressed(ControllerButtons button);
        public abstract bool IsButtonHeld(ControllerButtons button);
        public abstract bool IsButtonReleased(ControllerButtons button);

    }

    /// <summary>
    /// All of the controller buttons in the game
    /// </summary>
    public enum ControllerButtons
    {
        Default = 0,
        Up = 1,
        Right = 2,
        Down = 3,
        Left = 4,
        Y = 5,
        B = 6,
        A = 7,
        X = 8,
        Start = 9,
        Select = 10,
    }

    /// <summary>
    /// The key mappings used in the game, this is used to set the appropriate keys for keymapping
    /// </summary>
    /// <typeparam name="T">The type of keymapping, should be a button or a Key</typeparam>
    public class KeyMapping<T>
    {
        public T UpButton;
        public T RightButton;
        public T DownButton;
        public T LeftButton;
        public T YButton;
        public T BButton;
        public T AButton;
        public T XButton;
        public T StartButton;
        public T SelectButton;

    }
}