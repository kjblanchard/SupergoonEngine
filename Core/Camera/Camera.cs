////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core.Input;
using SgEngine.EKS;

namespace SgEngine.Core.Camera
{
    public class Camera
    {
        private static Vector3 _location;
        private static ResolutionHelper _resolutionHelper;
        private static GraphicsDevice _graphicsDevice;
        //TODO this is here for testing moving camera, it can be removed later when testing is not needed
        private PlayerController _currentController;
        /// <summary>
        /// The main Game camera.  This is used to move around the current map.
        /// </summary>
        /// <param name="resolutionHelper">The resolution help that it works with</param>
        /// <param name="graphicsDevice">The graphics device that is used for calculations</param>
        public Camera(ResolutionHelper resolutionHelper, GraphicsDevice graphicsDevice)
        {
            _resolutionHelper = resolutionHelper;
            _graphicsDevice = graphicsDevice;
            _location = Vector3.Zero;
            _currentController = GameWorld.GetPlayerController(0);
        }

        public void SetPlayerController(PlayerController playerController)
        {
            _currentController = playerController;
        }

        public void Update()
        {
            if (_currentController.IsButtonHeld(ControllerButtons.Right))
                _location.X += 1;
            if (_currentController.IsButtonHeld(ControllerButtons.Left))
                _location.X -= 1;
            if (_currentController.IsButtonHeld(ControllerButtons.Up))
                _location.Y -= 1;
            if (_currentController.IsButtonHeld(ControllerButtons.Down))
                _location.Y += 1;
        }
        public static Vector2 ScreenToWorld(Vector2 screenPosition)
        {
            var viewportTopLeft = new Vector2(_graphicsDevice.Viewport.X, _graphicsDevice.Viewport.Y);
            var screenToWorldScale = _resolutionHelper.WorldSize.X / (float)_graphicsDevice.Viewport.Width;
            return (screenPosition - viewportTopLeft) * screenToWorldScale;
        }

        public static Vector2 CalculateCameraOffset(Vector2 currentLocation)
        {

            currentLocation.X -= _location.X;
            currentLocation.Y -= _location.Y;
            return currentLocation;
        }

        public static Vector2 ScreenToWorldAndCamOffset(Vector2 currentLocation)
        {
            var locAndCamOffset = CalculateCameraOffset(currentLocation);

            return ScreenToWorld(locAndCamOffset);
        }
        public Matrix GetCameraTransformMatrix()
        {
            Matrix.CreateTranslation(ref _location, out var matrix);
            return matrix;
        }
    }
}