////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.GUI.Components;

namespace SgEngine.GUI
{
    public class GuiUiImage : GuiUiComponent
    {
        private GuiUiImageComponent _guiUiImageComponent;

        public GuiUiImage(Enum spriteSheetToLoad, Point size, Vector2 location = new Vector2()) : base(location,size)
        {
            _guiUiImageComponent = new GuiUiImageComponent(this, spriteSheetToLoad, size);
        }
        public override void Initialize()
        {
            base.Initialize();
            _guiUiImageComponent.Initialize();
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _guiUiImageComponent.Update(gameTime);
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            _guiUiImageComponent.Draw(gameTime,spriteBatch);
        }
    }
}