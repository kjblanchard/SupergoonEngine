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
        public Rectangle SpriteRectangle;
        public int currentSheetIndex;
        private readonly int sheetColumn;
        private readonly int sheedRows;
        public int Height;
        public int Width;
        public bool IsLoaded = false;
        public string _fileName;
        private bool depth;

        public Spritesheet(string fileName)
        {
            _fileName = fileName;
        }

        public Vector2 Center()
        {
            Vector2 newVector2 = new Vector2();
            newVector2.X = Width / 2;
            newVector2.Y = Height / 2;
            return newVector2;
        }
    }
}