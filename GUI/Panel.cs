////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.Core.Input;
using SgEngine.EKS;
using SgEngine.GUI.Components;

namespace SgEngine.GUI
{
    public class Panel : GuiComponent
    {
        public new bool DebugMode
        {
            get => _debugMode;
            set
            {
                if (_guiImageComponent != null)
                {
                    _guiImageComponent.DebugMode = value;
                }
                _debugMode = value; 

            }
        }

        private readonly List<GuiComponent> _allComponents = new List<GuiComponent>();
        private GuiImageComponent _guiImageComponent;
        private PlayerController _controller;
        public Panel(Vector2 location = new Vector2(), Point size = new Point(), Enum spriteSheetToLoad = null) : base(location, size)
        {
            if (spriteSheetToLoad != null)
            {
                _guiImageComponent = new GuiImageComponent(this, spriteSheetToLoad, size);
            }
        }

        public override void Initialize()
        {
            base.Initialize();
            _guiImageComponent?.Initialize();
        }

        public void AddUiObject(GuiComponent guiObject)
        {
            _controller = GameWorld.GetPlayerController(0);
            guiObject.Initialize();
            guiObject.LoadContent();
            _allComponents.Add(guiObject);
        }

        public void ModifyDebugForPanel(bool debugValue)
        {
            foreach (var _allComponent in _allComponents)
            {
                _allComponent.DebugMode = debugValue;
                DebugMode = debugValue;
            }

        }
        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _guiImageComponent?.Update(gameTime);
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
            _guiImageComponent?.Draw(gameTime, spriteBatch);
            //TODO testing drawing panels and stuff
            var controllerRect = new RectangleF(Controller.MouseScreenToWorldResolution(), new Vector2(16, 16));
            spriteBatch.DrawRectangle(controllerRect, Color.White);

            base.Draw(gameTime, spriteBatch);
            foreach (var _allComponent in _allComponents)
            {

                //spriteBatch.DrawRectangle(_allComponent.BoundingBoxAfterOrigin, Color.Blue);
                _allComponent.Draw(gameTime, spriteBatch);
            }
        }
    }
}