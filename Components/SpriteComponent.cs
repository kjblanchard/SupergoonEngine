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
using SgEngine.Core;
using SgEngine.EKS;

namespace SgEngine.Components
{
    /// <summary>
    /// The component that should be used for all gameobjects that need to draw to the screen
    /// </summary>
    public class SpriteComponent : Component
    {

        #region State

        private GraphicsDevice _graphics;
        private bool _debugMode = true;
        private readonly bool _isWholeTexture = true;
        private Point _size;
        private readonly Rectangle _offsetAndSize;
        public float Opacity { get; set; } = 1f;
        private readonly Spritesheet _spriteSheet;

        #endregion

        #region Constructor

        public SpriteComponent(GameObject parent, Enum objectLoLoad) : base(parent)
        {
            _componentType = EngineComponentTypes.SpriteComponent;
            _parent = parent;
            _spriteSheet = ContentLoader.GetSpriteSheet(objectLoLoad);
            _graphics ??= GameWorld.GetGraphicsDevice();
        }
        public SpriteComponent(GameObject parent, Enum assetToLoad, Point size) : this(parent, assetToLoad)
        {
            _size = size;
            _offsetAndSize = new Rectangle(new Point(0, 0), size);
            _isWholeTexture = false;
        }

        public SpriteComponent(GameObject parent, Enum assetToLoad, Rectangle offsetAndSize) : this(parent,
            assetToLoad)
        {
            _offsetAndSize = offsetAndSize;
            _size = offsetAndSize.Size;
            _isWholeTexture = false;
        }

        #endregion

        #region Functions

        public override void Initialize()
        {
        }


        public override void LoadContent()
        {
            base.LoadContent();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            if (_isWholeTexture)
            {
                spriteBatch.Draw(_spriteSheet._texture, (_localPosition + _parent.LocalPosition),
                      Color.White * Opacity);
            }

            else
            {
                var temp = _offsetAndSize;
                temp.Location += _parent.LocalPosition.ToPoint();

                spriteBatch.Draw(_spriteSheet._texture, temp, null, Color.White * Opacity, 0.0f, _spriteSheet.Center(),
                    SpriteEffects.None, 0.0f);
                if (_debugMode)
                    DrawDebugBox(spriteBatch, temp);
            }
        }

        private void DrawDebugBox(SpriteBatch spriteBatch, Rectangle positionToDraw)
        {
            var size = _offsetAndSize.Size;
            positionToDraw.X -= size.X / 2;
            positionToDraw.Y -= size.Y / 2;
            spriteBatch.DrawRectangle(positionToDraw, Color.Aqua);

        }
        #endregion
    }
}