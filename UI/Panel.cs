////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.UI
{
    public class Panel : UIComponent
    {
        private Rectangle _locationAndSize;
        private List<UIComponent> allComponents = new List<UIComponent>();
        public Panel(Rectangle locationAndSize) : base(locationAndSize.Location.ToVector2())
        {
            _locationAndSize = locationAndSize;
        }
        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            foreach (var _allComponent in allComponents)
            {
                _allComponent.Update(gameTime);
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            foreach (var _allComponent in allComponents)
            {
                _allComponent.Draw(gameTime,spriteBatch);
            }
        }
    }
}