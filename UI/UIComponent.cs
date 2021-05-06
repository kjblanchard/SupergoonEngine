////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
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

        public Rectangle BoundingBoxAfterOrigin
        {
            get
            {
                var tempValue = GlobalPosition.ToPoint();
                tempValue -= new Point(_size.X / 2, _size.Y / 2);
                return new Rectangle(tempValue, _size);

            }
        }

        public Point _size;

        public Vector2 LocalPosition = Vector2.Zero;
        public bool IsActive;
        protected UIComponent _parent;
        protected Enum ComponentType;
        public bool _debugMode;
        public bool _isSelected;
        protected UIComponent(Vector2 localPosition, Point size = new Point(), UIComponent parent = null)
        {
            _parent = parent;
            LocalPosition = localPosition;
            _size = size;
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