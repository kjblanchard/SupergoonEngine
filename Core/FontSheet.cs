////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Core
{
    public class FontSheet
    {

        public SpriteFont SpriteFont;
        public string FileName;
        public bool IsLoaded;
        public int OneLineYSize;

        public FontSheet(string fileToLoad, int oneLineSize)
        {
            FileName = fileToLoad;
            OneLineYSize = oneLineSize;
        }
    }
}