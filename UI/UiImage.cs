////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.UI.Components;

namespace SgEngine.UI
{
    public class UiImage : UIComponent
    {
        private UiImageComponent _uiImageComponent;

        public UiImage(Enum spriteSheetToLoad, Point size, Vector2 location = new Vector2()) : base(location)
        {
            _uiImageComponent = new UiImageComponent(this, spriteSheetToLoad, size, location);
        }
        public override void Initialize()
        {
            base.Initialize();
            _uiImageComponent.Initialize();
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _uiImageComponent.Update(gameTime);
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            _uiImageComponent.Draw(gameTime,spriteBatch);
        }
    }
}