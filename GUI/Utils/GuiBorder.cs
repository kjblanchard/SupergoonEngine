////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

namespace SgEngine.GUI.Utils
{
    /// <summary>
    /// Used in Gui components for measuring the screen sides to offset things.
    /// </summary>
    public struct GuiBorder
    {
        public int Top;
        public int Right;
        public int Bottom;
        public int Left;

        public GuiBorder(int allSides)
        {
            Top = Right = Bottom = Left = allSides;
        }
        public GuiBorder(int topBottom, int leftRight)
        {
            Top = Bottom = topBottom;
            Left = Right = leftRight;
        }
    }
}