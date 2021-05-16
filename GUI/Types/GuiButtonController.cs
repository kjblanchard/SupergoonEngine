////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using SgEngine.Core.Input;
using SgEngine.GUI.Components;

namespace SgEngine.GUI.Types
{
    public class GuiButtonController : GuiComponent
    {
        private int currentSelectedButton = 99;
        public List<GuiButton> ButtonsToManage = new List<GuiButton>();
        public GuiButtonController(Vector2 offset, Point size = new Point(), GuiComponent parent = null) : base(offset, size, parent)
        {
        }

        public void AddButton(GuiButton buttonToAdd)
        {
            ButtonsToManage.Add(buttonToAdd);
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            var mousePosition = Controller.MouseScreenCameraPosition();
            var mouseRect = new RectangleF(mousePosition, new Size2(16, 16));
            currentSelectedButton = 99;
            for (var i = 0; i < ButtonsToManage.Count ; i++)
            {
                var _guiButton = ButtonsToManage[i];
                if (!_guiButton.CheckIfHovered(mouseRect)) continue;
                currentSelectedButton = i;
                break;
            }
            HandleCurrentSelection();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            foreach (var _guiButton in ButtonsToManage)
            {
                _guiButton.Draw(gameTime,spriteBatch);
            }
        }

        private void HandleCurrentSelection()
        {

            for (int i = 0; i < ButtonsToManage.Count; i++)
            {
                if(i != currentSelectedButton)
                    ButtonsToManage[i]._guiTextComponent.TextColor = Color.White;
                else
                    ButtonsToManage[i]._guiTextComponent.TextColor = Color.Blue;
            }
        }
    }
}