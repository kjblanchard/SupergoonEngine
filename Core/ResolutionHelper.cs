////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using FMOD;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended.Sprites;
using SgEngine.EKS;

namespace SgEngine.Core
{
    public class ResolutionHelper
    {
        public static Point windowSize;
        public static Point worldSize;
        public static GraphicsDeviceManager _graphics;
        public static GraphicsDevice _graphicsDevice;
        public static Matrix SpriteScale;

        public ResolutionHelper(GraphicsDeviceManager graphicsDeviceManager, GraphicsDevice GraphicsDevice)
        {
            _graphicsDevice = GraphicsDevice;
            _graphics = graphicsDeviceManager;

        }
        public void ApplyResolutionSettings(bool fullScreen)
        {
            _graphics.IsFullScreen = fullScreen;
            Point screenSize;
            if (fullScreen)
                screenSize = new Point(GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Width, GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Height);
            else
                screenSize = windowSize;

            // scale the window to the desired size
            _graphics.PreferredBackBufferWidth = screenSize.X;
            _graphics.PreferredBackBufferHeight = screenSize.Y;

            _graphics.ApplyChanges();

            // calculate and set the viewport to use
            _graphicsDevice.Viewport = CalculateViewport(screenSize);

            // calculate how the graphics should be scaled, so that the game world fits inside the window
            SpriteScale = Matrix.CreateScale((float)_graphicsDevice.Viewport.Width / worldSize.X, (float)_graphicsDevice.Viewport.Height / worldSize.Y, 1);
            //spriteScale.Translation = (new Vector3(100,0,0));
        }

        /// <summary>
        /// Calculates and returns the viewport to use, so that the game world fits on the screen while preserving its aspect ratio.
        /// </summary>
        /// <param name="windowSize">The size of the screen on which the world should be drawn.</param>
        /// <returns>A Viewport object that will show the game world as large as possible while preserving its aspect ratio.</returns>
        Viewport CalculateViewport(Point windowSize)
        {
            // create a Viewport object
            Viewport viewport = new Viewport();

            // calculate the two aspect ratios
            float gameAspectRatio = (float)worldSize.X / worldSize.Y;
            float windowAspectRatio = (float)windowSize.X / windowSize.Y;

            // if the window is relatively wide, use the full window height
            if (windowAspectRatio > gameAspectRatio)
            {
                viewport.Width = (int)(windowSize.Y * gameAspectRatio);
                viewport.Height = windowSize.Y;
            }
            // if the window is relatively high, use the full window width
            else
            {
                viewport.Width = windowSize.X;
                viewport.Height = (int)(windowSize.X / gameAspectRatio);
            }

            // calculate and store the top-left corner of the viewport
            viewport.X = (windowSize.X - viewport.Width) / 2;
            viewport.Y = (windowSize.Y - viewport.Height) / 2;

            return viewport;
        }


    }
}