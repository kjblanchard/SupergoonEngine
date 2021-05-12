////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.GUI.Components;

namespace SgEngine.GUI
{
    public class Gui : GuiComponent
    {
        public Gui(): base(new Vector2(), new Point())
        {
            
        }

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
                canvas.Initialize();
            }
        }

        public override void LoadContent()
        {
            foreach (var canvas in AllCanvasList)
            {
                canvas.LoadContent();
            }
        }
        
        public override void Update(GameTime gameTime)
        {
            foreach (var canvas in AllCanvasList)
            {
                canvas.Update(gameTime);
            }
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            foreach (var canvas in AllCanvasList)
            {
                canvas.Draw(gameTime,spriteBatch);
            }
        }
    }
    
    
}
