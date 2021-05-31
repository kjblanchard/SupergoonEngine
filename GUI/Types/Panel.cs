////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{
    public class Panel : GuiComponent
    {
        /// <summary>
        /// Sets the debug mode for itself and for its image component to the value
        /// </summary>
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

        /// <summary>
        /// Modifies the debugMode for the entire panel
        /// </summary>
        public bool PanelDebug
        {
            set
            {
                foreach (var _allComponent in _allComponents)
                {
                    _allComponent.DebugMode = value;
                }

                DebugMode = value;
            }
        }

        private readonly List<GuiComponent> _allComponents = new List<GuiComponent>();
        private readonly GuiImageComponent _guiImageComponent;
        /// <summary>
        /// Used when created in code
        /// </summary>
        /// <param name="location">The place to draw it</param>
        /// <param name="size">The size of this panel</param>
        /// <param name="spriteSheetToLoad">Will create a gui Image component on it if needed</param>
        public Panel(Vector2 location = new Vector2(), Point size = new Point(), Enum spriteSheetToLoad = null) : base(location, size)
        {
            if (spriteSheetToLoad != null)
            {
                _guiImageComponent = new GuiImageComponent(this, spriteSheetToLoad, size);
            }
        }
        /// <summary>
        /// Used when loaded from the config reader
        /// </summary>
        /// <param name="location"></param>
        /// <param name="size"></param>
        /// <param name="spritesheetToLoad"></param>
        public Panel(Vector2 location, Point size, int spriteSheetToLoad) : base(location, size)
        {
            _guiImageComponent = new GuiImageComponent(this, spriteSheetToLoad, size);
        }

        public override void Initialize()
        {
            base.Initialize();
            _guiImageComponent?.Initialize();
        }

        public void AddGuiComponent(GuiComponent guiObject)
        {
            guiObject.Initialize();
            guiObject.LoadContent();
            guiObject.BeginRun();
            _allComponents.Add(guiObject);
        }
        public void AddGuiComponent(params GuiComponent[] guiObjects)
        {
            foreach (var guiComponent in guiObjects)
            {
                AddGuiComponent(guiComponent);
            }
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _guiImageComponent?.Update(gameTime);
            foreach (var _allComponent in _allComponents)
            {
                _allComponent.Update(gameTime);
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            _guiImageComponent?.Draw(gameTime, spriteBatch);
            base.Draw(gameTime, spriteBatch);
            foreach (var _allComponent in _allComponents)
            {
                _allComponent.Draw(gameTime, spriteBatch);
            }
        }
    }
}