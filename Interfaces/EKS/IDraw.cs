////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Interfaces.EKS
{
    public interface IDraw
    {
        
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch){}
    }
}