////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{
    public abstract class GuiButton : GuiComponent
    {
        protected TextBoxConfig _textBoxConfig;
        public GuiTextComponent _guiTextComponent;
        protected GuiImageComponent _guiImageComponent;
        protected bool _isHovered;
        protected GuiButton(TextBoxConfig textBoxConfig, Point size, Vector2 parentOffset, Enum graphicToLoad = null) : base(textBoxConfig.parentOffset, textBoxConfig.textBoxSize, textBoxConfig.parent)
        {
            _textBoxConfig = textBoxConfig;
            if (graphicToLoad != null)
                _guiImageComponent = new GuiImageComponent(this, graphicToLoad, size, parentOffset);
            _guiTextComponent = new GuiTextComponent(textBoxConfig);
            _guiTextComponent.Initialize();
            _guiImageComponent.Initialize();
        }

        public bool CheckIfHovered(Rectangle thingToCheckAgainst)
        {
            return Collision.Collision.ShapesIntersect(thingToCheckAgainst, BoundingBox);
        }
        public bool CheckIfHovered(RectangleF thingToCheckAgainst)
        {
            return Collision.Collision.ShapesIntersect(thingToCheckAgainst, BoundingBox);
        }

        public virtual void OnHover()
        {
            _isHovered = true;
        }

        public virtual void OnHoverLeave()
        {
            _isHovered = false;
        }
        public virtual void OnClick() { }
        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            if(_debugMode)
                DrawDebugBox(spriteBatch,BoundingBox);
        }

        /// <summary>
        /// Draws a Debug box around the text
        /// </summary>
        /// <param name="spriteBatch">The spritebatch to draw to</param>
        /// <param name="positionToDraw">The position to draw on the screen</param>
        /// <param name="textOrigin">The origin of the text, so that we can rotate it and stuff</param>
        private void DrawDebugBox(SpriteBatch spriteBatch, RectangleF positionToDraw)
        {
            spriteBatch.DrawRectangle(positionToDraw, Color.Red);
        }
    }
}