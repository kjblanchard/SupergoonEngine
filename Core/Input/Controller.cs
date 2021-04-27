////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;

namespace SgEngine.Core.Input
{
    public abstract class Controller
    {
        protected static KeyMapping<Keys> DefaultKeyboardMap = new KeyMapping<Keys>()
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

        protected static KeyMapping<Buttons> DefaultJoystickMap = new KeyMapping<Buttons>
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

        protected InputSg _input = GameWorld.Input;

        public abstract bool IsButtonPressed(ControllerButtons button);
        public abstract bool IsButtonHeld(ControllerButtons button);
        public abstract bool IsButtonReleased(ControllerButtons button);

    }

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