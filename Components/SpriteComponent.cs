﻿////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core;
using SgEngine.EKS;

namespace SgEngine.Components
{
    public class SpriteComponent : Component
    {

        #region State

        bool _isWholeTexture = true;
        private Point _size;
        private Rectangle _offsetAndSize;
        public float Opacity { get; set; } = 1f;
        public Spritesheet _spritesheet;

        #endregion

        #region Constructor

        public SpriteComponent(GameObject parent, Enum objectLoLoad) : base(parent)
        {
            _componentType = EngineComponentTypes.SpriteComponent;
            _parent = parent;
            _spritesheet = ContentLoader.GetSpriteSheet(objectLoLoad);
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
                spriteBatch.Draw(_spritesheet._texture, (_localPosition + _parent.LocalPosition), Color.White * Opacity);
            else
            {
                var temp = _offsetAndSize;
                temp.Location += _parent.LocalPosition.ToPoint();

                spriteBatch.Draw(_spritesheet._texture, temp, null, Color.White, 0.0f, _spritesheet.Center(),
                    SpriteEffects.None, 0.0f);

            }
        }



        #endregion
    }
}