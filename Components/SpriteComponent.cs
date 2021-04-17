////////////////////////////////////////////////////////////
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

        private Texture2D _loadedTexture;
        bool _isWholeTexture = true;
        private Point _size;
        private Rectangle _offsetAndSize;

        #endregion

        #region Constructor

        public SpriteComponent(GameObject parent, string assetToLoad) : base(parent)
        {
            _componentType = EngineComponentTypes.SpriteComponent;
            _parent = parent;
            _loadedTexture = ContentLoader.LoadSprite(assetToLoad);

        }
        public SpriteComponent(GameObject parent, string assetToLoad, Point size) : this(parent, assetToLoad)
        {
            _size = size;
            _isWholeTexture = false;
        }

        public SpriteComponent(GameObject parent, string assetToLoad, Rectangle offsetAndSize) : this(parent,
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
            _offsetAndSize = new Rectangle(_localPosition.ToPoint() + _parent.LocalPosition.ToPoint(), _offsetAndSize.Size);
        }


        public override void LoadContent()
        {
            base.LoadContent();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            if (_isWholeTexture)
                spriteBatch.Draw(_loadedTexture, (_localPosition + _parent.LocalPosition), Color.White);
            else
            {
                var temp = _offsetAndSize;
                temp.Location += _parent.LocalPosition.ToPoint();
                spriteBatch.Draw(_loadedTexture, temp, Color.White);
            }
        }

        #endregion

    }
}