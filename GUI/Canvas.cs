////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.GUI
{
    public class Canvas : GuiUiComponent
    {
        private List<Panel> AllPanels = new List<Panel>();
        private Rectangle _locationAndSize;

        public Canvas(Rectangle locationAndSize) : base(locationAndSize.Location.ToVector2())
        {
            _locationAndSize = locationAndSize;
        }

        public override void Initialize()
        {
            base.Initialize();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            foreach (var _allPanel in AllPanels)
            {
                _allPanel.Draw(gameTime,spriteBatch);
            }
        }

        public override void Update(GameTime gameTime)
        {
            foreach (var _panel in AllPanels)
            {
                _panel.Update(gameTime);
            }
        }


        public void AddPanel(Panel panelToAdd)
        {
            AllPanels.Add(panelToAdd);
            panelToAdd.Initialize();
            panelToAdd.LoadContent();
            panelToAdd.BeginRun();
        }

        public void RemovePanel(Panel panelToRemove)
        {
            panelToRemove.Deactivate();
            AllPanels.Remove(panelToRemove);
        }
    }
}
