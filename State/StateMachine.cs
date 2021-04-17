using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Interfaces;

namespace SgEngine.State
{
    public class StateMachine <T> : IUpdate where T : IState
    {
        #region Configuration

        readonly Dictionary<Enum, T> _gameStates = new Dictionary<Enum, T>();


        protected T _currentState;
        protected T _previousState;

        #endregion


        #region Methods

        public StateMachine()
        {
        }

        public virtual void Initialize()
        {
        }

        public virtual void LoadContent()
        {
        }

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
            }
        }

        public virtual void AddGameState(Enum name, T state)
        {
            _gameStates[name] = state;
        }

        public void ChangeStateToPreviousState()
        {
            _currentState.End();
            _currentState = _previousState;
            _currentState.Initialize();
            _currentState.LoadContent();
        }

        public void Update(GameTime gameTime)
        {
            _currentState.Update(gameTime);
        }

        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            _currentState.Draw(gameTime, spriteBatch);
        }

        public void Reset()
        {
            _currentState.Reset();
        }

        #endregion
    }
    
}
