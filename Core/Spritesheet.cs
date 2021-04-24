////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Core
{
    public class Spritesheet
    {
        public Texture2D _texture;
        public Point origin;
        public Rectangle SpriteRectangle;
        public int currentSheetIndex;
        private readonly int sheetColumns;
        private readonly int sheedRows;

        public bool IsLoaded = false;
        public string _fileName;
        private bool depth;

        public Spritesheet(string fileName)
        {
            _fileName = fileName;
        }
    }
}