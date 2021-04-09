using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Interfaces
{
    interface IUpdate
    {
        virtual void Startup(){}

        virtual void Update(GameTime gameTime){}
        virtual void HandleInput (){}

        virtual void Draw(SpriteBatch spriteBatch){}
    }
}
