﻿////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.Components;
using SgEngine.Core;

namespace SgEngine.GUI.Components
{
    public class GuiImageComponent : GuiComponent
    {
        public bool IsVisible { get; set; } = true;
        private readonly Spritesheet _spriteSheet;
        private float _opacity = 1.0f;
        private Rectangle drawRectangle;

        public GuiImageComponent(GuiComponent parent, Enum spriteSheetToLoad, Point size, Vector2 offset = new Vector2()) : base(offset, size, parent)
        {
            _componentType = EngineComponentTypes.UiComponents.ImageComponent;
            _spriteSheet = ContentLoader.GetSpriteSheet(spriteSheetToLoad);
            _size = size;
        }

        public override void Initialize()
        {
            drawRectangle = new Rectangle(GlobalPosition.ToPoint(), _size);
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            drawRectangle.Location = LocationOverride ? LocalPosition.ToPoint() : GlobalPosition.ToPoint();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            if (!IsVisible)
                return;
            //var rectToDraw = new Rectangle(GlobalPosition.ToPoint(), _size);
            //if (LocationOverride)
            //    rectToDraw.Location = LocalPosition.ToPoint();
            spriteBatch.Draw(_spriteSheet._texture, drawRectangle, null,
                Color.White * _opacity, 0.0f, _spriteSheet.Center(),
               SpriteEffects.None, 0.0f);
            if (_debugMode)
                DrawDebugBox(spriteBatch, drawRectangle);
        }
        private void DrawDebugBox(SpriteBatch spriteBatch, Rectangle positionToDraw)
        {
            positionToDraw.X -= _size.X / 2;
            positionToDraw.Y -= _size.Y / 2;
            spriteBatch.DrawRectangle(positionToDraw, Color.Honeydew);
        }
    }
}