////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using MonoGame.Extended;

namespace SgEngine.Collision
{
    public class Collision
    {

        public static Rectangle CalculateIntersection(Rectangle rect1, Rectangle rect2)
        {
            if (!ShapesIntersect(rect1, rect2))
                return new Rectangle(0, 0, 0, 0);

            int xmin = Math.Max(rect1.Left, rect2.Left);
            int xmax = Math.Min(rect1.Right, rect2.Right);
            int ymin = Math.Max(rect1.Top, rect2.Top);
            int ymax = Math.Min(rect1.Bottom, rect2.Bottom);
            return new Rectangle(xmin, ymin, xmax - xmin, ymax - ymin);
        }

        public static bool ShapesIntersect(Rectangle rectangle1, Rectangle rectangle2)
        {
            return rectangle1.Intersects(rectangle2);

        }
        public static bool ShapesIntersect(Rectangle rectangle1, RectangleF rectangle2)
        {
            return rectangle2.Intersects(rectangle1);

        }
        public static bool ShapesIntersect(RectangleF rectangle1, RectangleF rectangle2)
        {
            return rectangle2.Intersects(rectangle1);

        }
    }
}