////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using FMOD;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.EKS;

namespace SgEngine.Core
{
    public class ResolutionHelper
    {
        public static Point windowSize;
        public static Point worldSize;
        public static GraphicsDeviceManager graphics;
        public static GraphicsDevice graphicsDevice;
        public static Matrix SpriteScale;

        public static void GetScalingMatrix()
        {
            var spriteScale = Matrix.CreateScale(
                (float) graphicsDevice.Viewport.Width / worldSize.X,
                (float) graphicsDevice.Viewport.Height / worldSize.Y, 1);
            SpriteScale = spriteScale;
        }
        public static void Init(GraphicsDeviceManager graphicsboi, GraphicsDevice graphicsDev)
        {
            graphics = graphicsboi;
            graphicsDevice = graphicsDev;
            ApplyResolutionSettings();

        }

        public static void ToggleFullScreen()
        {
            graphics.IsFullScreen = !graphics.IsFullScreen;
            ApplyResolutionSettings();
        }

        public static void ApplyResolutionSettings()
        {
            var screenSize = new Point();
            if (graphics.IsFullScreen)
            {
                screenSize = new Point(GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Width,
                    GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Height);
            }
            else
            {
                var windowSize = new Point();
                windowSize.X = GameWorld._baseConfig.Window.X;
                windowSize.Y = GameWorld._baseConfig.Window.Y;
                screenSize = windowSize;

            }

            graphics.PreferredBackBufferWidth = screenSize.X;
            graphics.PreferredBackBufferHeight = screenSize.Y;
            windowSize = screenSize;
            graphicsDevice.Viewport = CalculateViewport(screenSize);
            GetScalingMatrix();
            graphics.ApplyChanges();
        }

        static Viewport CalculateViewport(Point windowsize)
        {
            var viewport = new Viewport();
            float gameAspectRatio = (float) worldSize.X / worldSize.Y;
            float windowAspectRatio = (float) windowsize.X / windowsize.Y;

            if (windowAspectRatio > gameAspectRatio)
            {
                viewport.Width = (int) (windowsize.Y * gameAspectRatio);
                viewport.Height = windowsize.Y;
            }
            else
            {
                viewport.Width = windowsize.X;
                viewport.Height = (int) (windowsize.X / gameAspectRatio);
            }

            viewport.X = (windowsize.X - viewport.Width) / 2;
            viewport.Y = (windowsize.Y - viewport.Height) / 2;
            return viewport;
        } 

    }
}