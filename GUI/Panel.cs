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
        private readonly List<GuiUiComponent> _allComponents = new List<GuiUiComponent>();
        private PlayerController _controller;
        public Panel(Vector2 location = new Vector2(), Point size = new Point()) : base(location,size)
        {
        }

        public void AddUiObject(GuiUiComponent guiUiObject)
        {
            _controller = GameWorld.GetPlayerController(0);
            guiUiObject.Initialize();
            guiUiObject.LoadContent();
            _allComponents.Add(guiUiObject);
        }
        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            var controllerRect = Controller.MouseScreenToWorldResolution().ToPoint();
            var realControllerRect = new Rectangle(controllerRect, new Point(25, 25));

            foreach (var _allComponent in _allComponents)
            {
                _allComponent.Update(gameTime);
                //TODO testing collisions, this section can be removed later
                if (!Controller.LeftMouseButtonClicked()) continue;
                if (Collision.Collision.ShapesIntersect(_allComponent.BoundingBoxAfterOrigin, realControllerRect))
                {
                    Debug.WriteLine("WOW an overlap");
                }
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            //TODO testing drawing panels and stuff
            var realControllerRect = new RectangleF(Controller.MouseScreenToWorldResolution(), new Vector2(16, 16));
            spriteBatch.DrawRectangle(realControllerRect, Color.White);

            base.Draw(gameTime, spriteBatch);
            foreach (var _allComponent in _allComponents)
            {

                spriteBatch.DrawRectangle(_allComponent.BoundingBoxAfterOrigin, Color.Blue);
                _allComponent.Draw(gameTime, spriteBatch);
            }
        }
    }
}