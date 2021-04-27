////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;

namespace SgEngine.Core.Input
{
    public class InputSg
    {
        private KeyboardState _currentKeyboardState, _previousKeyboardState;
        private GamePadState _playerOneCurrentJoystickState, _playerOnePreviousJoystickState;

        private GamePadState[] _previousGamePadStates = new GamePadState[4];
        private GamePadState[] _currentGamepadStates = new GamePadState[4];

        public InputSg()
        {
        }

        public void Update()
        {
            _previousKeyboardState = _currentKeyboardState;
            _currentKeyboardState = Keyboard.GetState();
            _currentGamepadStates.CopyTo(_previousGamePadStates, 0);
            for (int i = 0; i < _currentGamepadStates.Length; i++)
            {
                _currentGamepadStates[i] = GamePad.GetState((PlayerIndex)(i));
            }
        }
        /// <summary>
        /// Checks and returns whether the player has started pressing a certain keyboard key in the last frame of the game loop.
        /// </summary>
        /// <param name="k">The key to check.</param>
        /// <returns>true if the given key is now pressed and was not yet pressed in the previous frame; false otherwise.</returns>
        public bool KeyPressed(Keys k)
        {
            return _currentKeyboardState.IsKeyDown(k) && _previousKeyboardState.IsKeyUp(k);
        }

        public bool KeyPressed(int playerNumber, Buttons button)
        {
            return _currentGamepadStates[playerNumber].IsButtonDown(button) &&
                   _previousGamePadStates[playerNumber].IsButtonUp(button);
        }

        /// <summary>
        /// Checks and returns whether the player has stopped pressing a certain keyboard key in the last frame of the game loop.
        /// </summary>
        /// <param name="k">The key to check.</param>
        /// <returns>true if the given key is no longer pressed but was still pressed in the previous frame; false otherwise.</returns>
        public bool KeyReleased(Keys k)
        {
            return _currentKeyboardState.IsKeyUp(k) && _previousKeyboardState.IsKeyDown(k);
        }

        /// <summary>
        /// Checks and returns whether the player is currently holding a certain keyboard key down.
        /// </summary>
        /// <param name="k">The key to check.</param>
        /// <returns>true if the given key is currently being held down; false otherwise.</returns>
        public bool KeyDown(Keys k)
        {
            return _currentKeyboardState.IsKeyDown(k);
        }
    }
}