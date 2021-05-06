////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.UI
{
    public class UI : UIComponent
    {
        private List<Canvas> AllCanvasList = new List<Canvas>();
        public Canvas MasterCanvas = new Canvas(new Rectangle());
        public void AddPanel(Canvas panelToAdd)
        {
            AllCanvasList.Add(panelToAdd);
        }

        public void RemovePanel(Canvas panelToRemove)
        {
            AllCanvasList.Remove(panelToRemove);
        }

        public override void Initialize()
        {
            AllCanvasList.Add(MasterCanvas);
            foreach (var canvas in AllCanvasList)
            {
            }
        }

        public override void LoadContent()
        {
            foreach (var canvas in AllCanvasList)
            {
            }
        }
        
        public override void Update(GameTime gameTime)
        {
            foreach (var canvas in AllCanvasList)
            {
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            foreach (var canvas in AllCanvasList)
            {
            }
        }
    }
    
    
}
