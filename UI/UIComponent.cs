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
using IUpdate = SgEngine.Interfaces.IUpdate;

namespace SgEngine.UI
{
    public abstract class UIComponent : IUpdate
    {
        public Vector2 GlobalPosition
        {
            get
            {
                if (_parent == null)
                    return LocalPosition;
                return _parent.GlobalPosition + LocalPosition;
            }
        }

        public Vector2 LocalPosition = Vector2.Zero;
        public bool IsActive;
        protected UIComponent _parent;
        protected Enum ComponentType;
        public bool _debugMode;
        public bool _isSelected;
        protected UIComponent(UIComponent parent = null)
        {
            _parent = parent;
        }
        protected UIComponent(Vector2 localPosition, UIComponent parent = null) : this(parent)
        {
            LocalPosition = localPosition;
        }

        public virtual void Initialize()
        {
        }

        public virtual void LoadContent()
        {
        }

        public void BeginRun()
        {
            IsActive = true;
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
            IsActive = true;
            OnActivate();
        }

        public void Deactivate()
        {
            IsActive = false;
            OnDeactivate();
        }
    }
}