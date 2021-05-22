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
using SgEngine.Core.Input;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{
    public abstract class GuiButton : GuiComponent
    {
        public bool PreviouslyHoveredStatus, IsHovered;
        public bool WasJustHovered => IsHovered && !PreviouslyHoveredStatus;
        public bool WasJustLeftHovered => !IsHovered && PreviouslyHoveredStatus;

        protected GuiTextComponent _buttonTextComponent;
        protected GuiImageComponent _guiImageComponent;

        /// <summary>
        /// Gets a rectangleF from the buttons bounding box, and offsets it by its parents origin, used for debug box drawing and
        /// for collision with the mouse, which is after origin draw
        /// </summary>
        protected RectangleF BoundingBoxParentOffset
        {
            get
            {
                var bounds = BoundingBox;
                bounds.Location -= _parent.Origin.ToPoint();
                return bounds;
            }
        }
        /// <summary>
        /// A gui button class that should be used as a base for all buttons
        /// </summary>
        /// <param name="textBoxConfig">The text that should be shown with the button</param>
        /// <param name="size">The size of the button</param>
        /// <param name="parentOffset">The offset of the components parent</param>
        /// <param name="parent">The parent of this object</param>
        /// <param name="graphicToLoad">The graphic for this button</param>
        protected GuiButton(Point size, Vector2 parentOffset, GuiComponent parent, TextBoxConfig textBoxConfig = null, Enum graphicToLoad = null) : base(parentOffset, size, parent)
        {
            if (graphicToLoad != null)
                _guiImageComponent = new GuiImageComponent(this, graphicToLoad, size, parentOffset);
            if (textBoxConfig != null)
                _buttonTextComponent = new GuiTextComponent(textBoxConfig,textBoxConfig.FontType);
        }

        public override void Initialize()
        {
            base.Initialize();
            _buttonTextComponent?.Initialize();
            _guiImageComponent?.Initialize();
        }
        /// <summary>
        /// Updates the Hovered Status based on the mousebounds of the controller
        /// </summary>
        public override void HandleInput()
        {
            base.HandleInput();
            UpdateHoveredStatus(Controller.MouseBounds());
        }
        private void UpdateHoveredStatus(RectangleF thingToCheckAgainst)
        {
            PreviouslyHoveredStatus = IsHovered;
            IsHovered = CheckIfHovered(thingToCheckAgainst);
            if (WasJustHovered)
                OnJustHovered();
            if (WasJustLeftHovered)
                OnJustHoveredLeave();
        }
        private bool CheckIfHovered(RectangleF thingToCheckAgainst)
        {
            return Collision.Collision.ShapesIntersect(thingToCheckAgainst, BoundingBoxParentOffset);
        }
        /// <summary>
        /// What happens when this button is just hovered
        /// </summary>
        public virtual void OnJustHovered() { }
        /// <summary>
        /// What happens when this button was just left it's hovered status
        /// </summary>
        public virtual void OnJustHoveredLeave() { }
        /// <summary>
        /// What happens when this button is clicked
        /// </summary>
        public virtual void OnClick() { }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            _buttonTextComponent?.Draw(gameTime, spriteBatch);
            _guiImageComponent?.Draw(gameTime,spriteBatch);
            DrawDebugBox(spriteBatch, BoundingBoxParentOffset, new Vector2(), Color.Red);
        }
    }

}