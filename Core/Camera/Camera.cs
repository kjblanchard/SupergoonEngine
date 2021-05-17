////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Runtime.CompilerServices;
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
        private readonly PlayerController _currentController;
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
        //TODO this is here for testing moving camera, it can be removed later when testing is not needed
        private void MoveCamera()
        {
            
            //if (_currentController.IsButtonHeld(ControllerButtons.Right))
            //    _location.X += 1;
            //if (_currentController.IsButtonHeld(ControllerButtons.Left))
            //    _location.X -= 1;
            //if (_currentController.IsButtonHeld(ControllerButtons.Up))
            //    _location.Y -= 1;
            //if (_currentController.IsButtonHeld(ControllerButtons.Down))
            //    _location.Y += 1;
        }
        public void Update()
        {
            MoveCamera();
        }
        /// <summary>
        /// Returns the resolution world coordinates for the current screen position, probably used for UI
        /// </summary>
        /// <param name="screenPosition">The current position on the screen</param>
        /// <returns>The position in the world</returns>
        public static Vector2 ScreenToWorldResolution(Vector2 screenPosition)
        {
            var viewportTopLeft = new Vector2(_graphicsDevice.Viewport.X, _graphicsDevice.Viewport.Y);
            var screenToWorldScale = _resolutionHelper.WorldSize.X / (float)_graphicsDevice.Viewport.Width;
            return (screenPosition - viewportTopLeft) * screenToWorldScale;
        }

        /// <summary>
        /// Probably used for rendering most game objects, this will add in the camera offset to the gameObject so that it's in the proper location
        /// </summary>
        /// <param name="currentLocation">The current location of the object in the game world</param>
        /// <returns>The current location of the object in the gameworld in regards to the camera</returns>
        public static Vector2 CalculateCameraOffset(Vector2 currentLocation)
        {
            currentLocation.X -= _location.X;
            currentLocation.Y -= _location.Y;
            return currentLocation;
        }

        /// <summary>
        /// Calculates the current position given translated from Screen to world, with the camera offset
        /// </summary>
        /// <param name="currentScreenLocation">The current screen location</param>
        /// <returns></returns>
        public static Vector2 ScreenToWorldAndCamOffset(Vector2 currentScreenLocation)
        {
            var locAndCamOffset = CalculateCameraOffset(currentScreenLocation);

            return ScreenToWorldResolution(locAndCamOffset);
        }
        /// <summary>
        /// Gets the transform matrix of the CameraTransform matrix
        /// </summary>
        /// <returns></returns>
        public Matrix GetCameraTransformMatrix()
        {
            Matrix.CreateTranslation(ref _location, out var matrix);
            return matrix;
        }
    }
}