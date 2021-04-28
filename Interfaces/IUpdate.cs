using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Interfaces
{
    public interface IUpdate
    {
        public virtual void Initialize(){}
        public virtual void LoadContent(){}
        public virtual void BeginRun(){}
        public virtual void Update(GameTime gameTime){}
        public virtual void HandleInput (){}
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch){}
        public virtual void OnActivate() { }
        public virtual void OnDeactivate() { }
    }
}
