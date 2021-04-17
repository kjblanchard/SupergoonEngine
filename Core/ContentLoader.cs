////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Core
{
    public class ContentLoader
    {
        static ContentManager _contentManager;

        public ContentLoader(ContentManager content)
        {
            _contentManager = content;
        }

        /// <summary>
        /// Loads and returns the sprite with the given asset name.
        /// </summary>
        /// <param name="assetName">The name of the asset to load.</param>
        /// <returns>A Texture2D object containing the loaded sprite.</returns>
        public static Texture2D LoadSprite(string assetName)
        {
            return _contentManager.Load<Texture2D>(assetName);
        }

        /// <summary>
        /// Loads and returns the font with the given asset name.
        /// </summary>
        /// <param name="assetName">The name of the asset to load.</param>
        /// <returns>A SpriteFont object containing the loaded font.</returns>
        public static SpriteFont LoadFont(string assetName)
        {
            return _contentManager.Load<SpriteFont>(assetName);
        }
    }
}