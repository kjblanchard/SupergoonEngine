////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using MonoGame.Extended;
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

        public static Vector2 MousePosition()
        {
            return _input.MousePosition();
        }
        public static Vector2 MouseScreenToWorldResolution()
        {
            return Camera.Camera.ScreenToWorldResolution(_input.MousePosition());
        }

        public static RectangleF MouseBounds()
        {

            return new RectangleF(Controller.MouseScreenToWorldResolution(),
                 new Size2(10, 10));
        }
        public static Vector2 MouseScreenCameraPosition()
        {

            return Camera.Camera.ScreenToWorldAndCamOffset(_input.MousePosition());
        }

        public static bool WasThereMouseMovement()
        {
            return _input.WasThereMouseMovement();
        }

        public static bool LeftMouseButtonClicked()
        {
            return _input.LeftMouseButtonClicked();
        }

        public bool RightMouseButtonClicked()
        {
            return _input.RightMouseButtonClicked();
        }
        public delegate void ButtonPressedEventHandler(object sender, ControllerButtons buttonForEvent,
            ButtonActions actionPerformed);

        public event ButtonPressedEventHandler OnAButtonPressed;
        public event ButtonPressedEventHandler OnAButtonHeld;
        public event ButtonPressedEventHandler OnAButtonReleased;
        public event ButtonPressedEventHandler OnBButtonPressed;
        public event ButtonPressedEventHandler OnBButtonHeld;
        public event ButtonPressedEventHandler OnBButtonReleased;
        public event ButtonPressedEventHandler OnXButtonPressed;
        public event ButtonPressedEventHandler OnXButtonHeld;
        public event ButtonPressedEventHandler OnXButtonReleased;
        public event ButtonPressedEventHandler OnYButtonPressed;
        public event ButtonPressedEventHandler OnYButtonHeld;
        public event ButtonPressedEventHandler OnYButtonReleased;
        public event ButtonPressedEventHandler OnUpButtonPressed;
        public event ButtonPressedEventHandler OnUpButtonHeld;
        public event ButtonPressedEventHandler OnUpButtonReleased;
        public event ButtonPressedEventHandler OnRightButtonPressed;
        public event ButtonPressedEventHandler OnRightButtonHeld;
        public event ButtonPressedEventHandler OnRightButtonReleased;
        public event ButtonPressedEventHandler OnDownButtonPressed;
        public event ButtonPressedEventHandler OnDownButtonHeld;
        public event ButtonPressedEventHandler OnDownButtonReleased;
        public event ButtonPressedEventHandler OnLeftButtonPressed;
        public event ButtonPressedEventHandler OnLeftButtonHeld;
        public event ButtonPressedEventHandler OnLeftButtonReleased;
        public event ButtonPressedEventHandler OnStartButtonPressed;
        public event ButtonPressedEventHandler OnStartButtonHeld;
        public event ButtonPressedEventHandler OnStartButtonReleased;
        public event ButtonPressedEventHandler OnSelectButtonPressed;
        public event ButtonPressedEventHandler OnSelectButtonHeld;
        public event ButtonPressedEventHandler OnSelectButtonReleased;

        protected void OnButtonPressed(ControllerButtons buttonPressed)
        {
            switch (buttonPressed)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    OnUpButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Right:
                    OnRightButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Down:
                    OnDownButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Left:
                    OnLeftButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Y:
                    OnYButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.B:
                    OnBButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.A:
                    OnAButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.X:
                    OnXButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Start:
                    OnStartButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                case ControllerButtons.Select:
                    OnSelectButtonPressed?.Invoke(this, ControllerButtons.A, ButtonActions.Pressed);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonPressed), buttonPressed, null);
            }
        }
        protected void OnButtonReleased(ControllerButtons buttonReleased)
        {
            switch (buttonReleased)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    OnUpButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Right:
                    OnRightButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Down:
                    OnDownButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Left:
                    OnLeftButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Y:
                    OnYButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.B:
                    OnBButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.A:
                    OnAButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.X:
                    OnXButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Start:
                    OnStartButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                case ControllerButtons.Select:
                    OnSelectButtonReleased?.Invoke(this, ControllerButtons.A, ButtonActions.Released);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonReleased), buttonReleased, null);
            }
        }
        protected void OnButtonHeld(ControllerButtons buttonHeld)
        {
            switch (buttonHeld)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    OnUpButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Right:
                    OnRightButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Down:
                    OnDownButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Left:
                    OnLeftButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Y:
                    OnYButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.B:
                    OnBButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.A:
                    OnAButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.X:
                    OnXButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Start:
                    OnStartButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                case ControllerButtons.Select:
                    OnSelectButtonHeld?.Invoke(this, ControllerButtons.A, ButtonActions.Held);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonHeld), buttonHeld, null);
            }
        }

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