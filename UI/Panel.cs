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
        private List<UIComponent> allComponents = new List<UIComponent>();
        public Panel() : base()
        {
        }

        public void AddUiObject(UIComponent uiObject)
        {
            uiObject.Initialize();
            uiObject.LoadContent();
            allComponents.Add(uiObject);
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