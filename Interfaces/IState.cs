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

        /// <summary>
        /// This method should only be called once, and should do the "work"
        /// </summary>
        public virtual void Initialize(){}
        /// <summary>
        /// This method should actually do the loading of the things in here.
        /// </summary>
        public virtual void LoadContent(){}

        /// <summary>
        /// This will be called every frame
        /// </summary>
        /// <param name="gameTime">This is the gametime that is going to be passed in, to know the time passed</param>
        public virtual void Update(GameTime gameTime)
        {
        }

        /// <summary>
        /// This is to draw to the spritebatch every frame
        /// </summary>
        /// <param name="gameTime">The gametime to be passed in</param>
        /// <param name="spriteBatch">The spritebatch that it will draw to</param>
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
        }
        /// <summary>
        /// Handles input, not sure if this will be used yet
        /// </summary>
        protected virtual void HandleInput (){}
        /// <summary>
        /// Should reset this back to default?  not sure if going to use
        /// </summary>
        public virtual void Reset()
        {
        }

        /// <summary>
        /// What will happen when this state is over with
        /// </summary>
        public virtual void End()
        {
        }

        #endregion
    }
    }

