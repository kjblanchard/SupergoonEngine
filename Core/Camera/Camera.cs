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
        public Vector3 Location;
        public PlayerController currentController;
        public Camera()
        {
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
            {
                Location.X += 1;
            }

            if (currentController.IsButtonHeld(ControllerButtons.Left))
                Location.X -= 1;
            if (currentController.IsButtonHeld(ControllerButtons.Up))
                Location.Y -= 1;
            if (currentController.IsButtonHeld(ControllerButtons.Down))
                Location.Y += 1;
        }
    }
}