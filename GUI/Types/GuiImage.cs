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
    public class GuiImage : GuiComponent
    {
        private GuiImageComponent _guiImageComponent;

        public GuiImage(Enum spriteSheetToLoad, Point size, Vector2 location = new Vector2()) : base(location,size)
        {
            _guiImageComponent = new GuiImageComponent(this, spriteSheetToLoad, size);
        }
        public override void Initialize()
        {
            base.Initialize();
            _guiImageComponent.Initialize();
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _guiImageComponent.Update(gameTime);
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            _guiImageComponent.Draw(gameTime,spriteBatch);
        }
    }
}