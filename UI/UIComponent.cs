////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using IUpdate = SgEngine.Interfaces.IUpdate;

namespace SgEngine.UI
{
    public abstract class UIComponent : IUpdate
    {
        public Vector2 Location;
        public bool isActive; 
        public UIComponent Parent;
        private List<UIComponent> OwnedComponents;

        protected UIComponent(Vector2 location = new Vector2())
        {
            Location = location;
        }

        public virtual void Initialize()
        {
        }

        public virtual void LoadContent()
        {
        }

        public void BeginRun()
        {
            isActive = true;
            OnActivate();
        }

        public virtual void Update(GameTime gameTime)
        {
        }

        public virtual void HandleInput()
        {
        }

        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
        }

        public virtual void OnActivate()
        {
        }

        public virtual void OnDeactivate()
        {
        }

        public void Activate()
        {
            isActive = true;
            OnActivate();
        }

        public void Deactivate()
        {
            isActive = false;
            OnDeactivate();
        }
    }
}