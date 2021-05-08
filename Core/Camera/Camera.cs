////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core.Input;

namespace SgEngine.Core.Camera
{
    public class Camera
    {
        public static Vector3 Location;
        public PlayerController currentController;
        public static ResolutionHelper ResolutionHelper;
        public Camera(ResolutionHelper resolutionHelper)
        {
            ResolutionHelper = resolutionHelper;
            Location = Vector3.Zero;
        }
        public Matrix GetTransformMatrix()
        {
            var matrix = Matrix.Identity;
            Matrix.CreateTranslation(ref Location, out matrix);
            return matrix;
        }

        public void SetPlayerController(PlayerController playerController)
        {
            currentController = playerController;
        }

        public void Update()
        {
            if (currentController.IsButtonHeld(ControllerButtons.Right))
                Location.X += 1;
            if (currentController.IsButtonHeld(ControllerButtons.Left))
                Location.X -= 1;
            if (currentController.IsButtonHeld(ControllerButtons.Up))
                Location.Y -= 1;
            if (currentController.IsButtonHeld(ControllerButtons.Down))
                Location.Y += 1;
        }
        public static Vector2 ScreenToWorld(Vector2 screenPosition)
        {
            var viewportTopLeft = new Vector2(ResolutionHelper._graphicsDevice.Viewport.X, ResolutionHelper._graphicsDevice.Viewport.Y);
            var screenToWorldScale = ResolutionHelper.worldSize.X / (float)ResolutionHelper._graphicsDevice.Viewport.Width;
            return (screenPosition - viewportTopLeft) * screenToWorldScale;
        }

        public static Vector2 CalculateCameraOffset(Vector2 currentLocation)
        {

            currentLocation.X -= Location.X;
            currentLocation.Y -= Location.Y;
            return currentLocation;
        }

        public static Vector2 ScreenToWorldAndCamOffset(Vector2 currentLocation)
        {
            var locAndCamOffset = CalculateCameraOffset(currentLocation);

            return ScreenToWorld(locAndCamOffset);
        }
    }
}