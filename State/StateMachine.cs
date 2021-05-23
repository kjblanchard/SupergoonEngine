using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Interfaces;
using SgEngine.Interfaces.EKS;

namespace SgEngine.State
{
    /// <summary>
    /// Statemachine that will be used for managing state changes for things.
    /// </summary>
    /// <typeparam name="T">The type of states that it will hold in it's dictionary</typeparam>
    public class StateMachine<T> : IFullEksObject where T : IState
    {
        #region Configuration

        private readonly Dictionary<Enum, T> _gameStates = new Dictionary<Enum, T>();


        protected T _currentState;
        protected T _previousState;

        #endregion


        #region Methods

        public StateMachine()
        {
        }

        /// <summary>
        /// This is ran once before loadcontent and should do most of the "work"
        /// </summary>
        public virtual void Initialize()
        {
        }

        /// <summary>
        /// This should be where the content is loaded at, if possible
        /// </summary>
        public virtual void LoadContent()
        {
        }

        /// <summary>
        /// This changes between states, and will run the end on the current state, then init, load, and beginrun the currentstate
        /// </summary>
        /// <param name="name">This is the enum for the lookup of the state</param>
        /// <param name="additionalArguments">If this stays implemented, add in additional args here.</param>
        public virtual void ChangeState(Enum name, int additionalArguments = 0)
        {
            if (_currentState != null)
            {
                _currentState.End();
                _previousState = _currentState;
            }

            if (_gameStates.ContainsKey(name))
            {
                _currentState = _gameStates[name];
                _currentState.Initialize();
                _currentState.LoadContent();
                _currentState.BeginRun();
            }
        }

        /// <summary>
        /// Add in a gamestate to the dictionary.  This will need to be done for the statemachine to work properly
        /// </summary>
        /// <param name="name">The enum which will be used as the key for lookups</param>
        /// <param name="state">The actual state to be used as the value for the lookup</param>
        public virtual void AddGameState(Enum name, T state)
        {
            _gameStates[name] = state;
        }

        /// <summary>
        /// Changes to the previous state.  Runs the init, load content, and begin run
        /// </summary>
        public void ChangeStateToPreviousState()
        {
            _currentState.End();
            _currentState = _previousState;
            _currentState.Initialize();
            _currentState.LoadContent();
            _currentState.BeginRun();
        }

        /// <summary>
        /// This is ran once every frame.
        /// </summary>
        /// <param name="gameTime">The gametime that will have the elapsed time available</param>
        public void Update(GameTime gameTime)
        {
            _currentState.Update(gameTime);
        }

        /// <summary>
        /// This will be drawn every frame if possible
        /// </summary>
        /// <param name="gameTime">The gametime that will be used if you need to see the elapsed time</param>
        /// <param name="spriteBatch">The spritebatch to actually draw to</param>
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            _currentState.Draw(gameTime, spriteBatch);
        }

        /// <summary>
        /// Runs the reset function on the current state
        /// </summary>
        public void Reset()
        {
            _currentState.Reset();
        }

        #endregion
    }

}
