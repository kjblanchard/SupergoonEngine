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
    public class GuiButton : GuiComponent
    {
        protected TextBoxConfig _textBoxConfig;
        public GuiTextComponent _guiTextComponent;
        protected GuiImageComponent _guiImageComponent;
        public bool PreviouslyHoveredStatus, IsHovered;
        public bool WasJustHovered => IsHovered && !PreviouslyHoveredStatus;
        public bool WasJustLeftHovered => !IsHovered && PreviouslyHoveredStatus;


        protected Rectangle BoundingBoxParentOffset
        {
            get
            {
                var bounds = BoundingBox;
                bounds.Location -= _parent.Origin.ToPoint();
                return bounds;
            }
        }
        public GuiButton(TextBoxConfig textBoxConfig, Point size, Vector2 parentOffset,GuiComponent parent = null, Enum graphicToLoad = null) : base(parentOffset, size, parent)
        {
            _textBoxConfig = textBoxConfig;
            if (graphicToLoad != null)
                _guiImageComponent = new GuiImageComponent(this, graphicToLoad, size, parentOffset);
            _guiTextComponent = new GuiTextComponent(textBoxConfig);
            _guiTextComponent.Initialize();
            if (graphicToLoad != null)
                _guiImageComponent.Initialize();
        }

        public void UpdateHoveredStatus(RectangleF thingToCheckAgainst)
        {
            PreviouslyHoveredStatus = IsHovered;
            IsHovered = CheckIfHovered(thingToCheckAgainst);
            if (WasJustHovered)
                OnJustHovered();
            if (WasJustLeftHovered)
                OnJustHoveredLeave();
        }


        public bool CheckIfHovered(Rectangle thingToCheckAgainst)
        {
            return Collision.Collision.ShapesIntersect(thingToCheckAgainst, BoundingBoxParentOffset);
        }
        public bool CheckIfHovered(RectangleF thingToCheckAgainst)
        {
            return Collision.Collision.ShapesIntersect(thingToCheckAgainst, BoundingBoxParentOffset);
        }

        public virtual void OnJustHovered()
        {
        }
        public virtual void OnJustHoveredLeave()
        {
        }
        public virtual void OnClick() { }
        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            _guiTextComponent.Draw(gameTime, spriteBatch);
            DrawDebugBox(spriteBatch, BoundingBoxParentOffset, new Vector2(),Color.Red);
        }

        public void AutoSetSizeBasedOnText()
        {
            _guiTextComponent.AutoSetSize();
        }

        public override void HandleInput()
        {
            base.HandleInput();
            UpdateHoveredStatus(Controller.MouseBounds());
        }

    }

}