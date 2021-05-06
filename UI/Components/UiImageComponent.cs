////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.Components;
using SgEngine.Core;

namespace SgEngine.UI.Components
{
    public class UiImageComponent : UIComponent
    {
        private GraphicsDevice _graphics;
        private readonly Spritesheet _spriteSheet;
        private Point _size;
        private float _opacity = 1.0f;

        public UiImageComponent(UIComponent parent, Enum spriteSheetToLoad, Point size, Vector2 location = new Vector2()) : base(location, parent)
        {
            ComponentType = EngineComponentTypes.UiComponents.ImageComponent;
            _spriteSheet = ContentLoader.GetSpriteSheet(spriteSheetToLoad);
            _size = size;
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            var rectToDraw = new Rectangle(GlobalPosition.ToPoint(), _size);
                spriteBatch.Draw(_spriteSheet._texture,rectToDraw, null,
                    Color.White * _opacity, 0.0f, _spriteSheet.Center(),
                    SpriteEffects.None, 0.0f);
                if (_debugMode)
                    DrawDebugBox(spriteBatch, rectToDraw);
        }
        private void DrawDebugBox(SpriteBatch spriteBatch, Rectangle positionToDraw)
        {
            positionToDraw.X -= _size.X / 2;
            positionToDraw.Y -= _size.Y / 2;
            spriteBatch.DrawRectangle(positionToDraw, Color.Honeydew);
        }
    }
}