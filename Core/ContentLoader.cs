////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Core
{
    public class ContentLoader
    {
        static ContentManager _contentManager;
        private static Spritesheet[] _spriteSheets;
        private static FontSheet[] _fontSheets;
        public ContentLoader(ContentManager content, Spritesheet[] spriteSheetDictionary,FontSheet[] fontSheets)
        {
            _contentManager = content;
            _spriteSheets = spriteSheetDictionary;
            _fontSheets = fontSheets;
        }

        public static FontSheet GetFontSheet(int fontSheetToLoad)
        {
            var fontSheet = _fontSheets[fontSheetToLoad];
            if (!fontSheet.IsLoaded)
            {
                fontSheet.SpriteFont = LoadFont(fontSheet.FileName);
                fontSheet.IsLoaded = true;
            }

            return fontSheet;
        }

        public static Spritesheet GetSpriteSheet(Enum fileToLoad)
        {

            var maybeInt = Convert.ToInt32(fileToLoad);
            var spriteSheet = _spriteSheets[maybeInt];
            if (!spriteSheet.IsLoaded)
            {
                spriteSheet._texture = LoadSprite(spriteSheet._fileName);
                spriteSheet.Height = spriteSheet._texture.Height;
                spriteSheet.Width = spriteSheet._texture.Width;
                spriteSheet.IsLoaded = true;
            }

            return spriteSheet;
        }
        public static Spritesheet GetSpriteSheet(int fileToLoad)
        {

            var spriteSheet = _spriteSheets[fileToLoad];
            if (!spriteSheet.IsLoaded)
            {
                spriteSheet._texture = LoadSprite(spriteSheet._fileName);
                spriteSheet.Height = spriteSheet._texture.Height;
                spriteSheet.Width = spriteSheet._texture.Width;
                spriteSheet.IsLoaded = true;
            }

            return spriteSheet;
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