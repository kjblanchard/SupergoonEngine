using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.EKS;

namespace SgEngine.Interfaces
{
    public interface IState
    {
        #region Configuration

        #endregion


        #region Methods

        public virtual void Initialize(){}
        public virtual void LoadContent(){}

        public virtual void Update(GameTime gameTime)
        {
        }

        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
        }
        protected virtual void HandleInput (){}
        public virtual void Reset()
        {
        }

        public virtual void End()
        {
        }

        #endregion
    }
    }

