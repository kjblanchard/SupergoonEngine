////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.UI
{
    public class Canvas : UIComponent
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
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
        }


        public void AddPanel(Panel panelToAdd)
        {
            AllPanels.Add(panelToAdd);
        }

        public void RemovePanel(Panel panelToRemove)
        {
            AllPanels.Remove(panelToRemove);
        }
    }
}