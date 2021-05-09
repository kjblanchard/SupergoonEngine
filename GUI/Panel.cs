////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.Core.Input;
using SgEngine.EKS;

namespace SgEngine.GUI
{
    public class Panel : GuiUiComponent
    {
        private List<GuiUiComponent> allComponents = new List<GuiUiComponent>();
        public PlayerController controller;
        public int counter;
        public Panel(Vector2 location = new Vector2(), Point size = new Point()) : base(location,size)
        {
        }

        public void AddUiObject(GuiUiComponent guiUiObject)
        {
            controller = GameWorld.GetPlayerController(0);
            guiUiObject.Initialize();
            guiUiObject.LoadContent();
            allComponents.Add(guiUiObject);
        }
        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);

            var controllerRect = Controller.MouseScreenCameraPosition().ToPoint();
            var realControllerRect = new Rectangle(controllerRect, new Point(25, 25));

            foreach (var _allComponent in allComponents)
            {
                _allComponent.Update(gameTime);
                if (Controller.LeftMouseButtonClicked())
                {

                    if (Collision.Collision.ShapesIntersect(_allComponent.BoundingBoxAfterOrigin, realControllerRect))
                    {
                        Debug.WriteLine("WOW an overlap" + counter);
                        counter++;
                    }
                }
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            var realControllerRect = new RectangleF(Controller.MouseScreenCameraPosition(), new Vector2(16, 16));
            spriteBatch.DrawRectangle(realControllerRect, Color.White);

            base.Draw(gameTime, spriteBatch);
            foreach (var _allComponent in allComponents)
            {

                spriteBatch.DrawRectangle(_allComponent.BoundingBoxAfterOrigin, Color.Blue);
                _allComponent.Draw(gameTime, spriteBatch);
            }
        }
    }
}