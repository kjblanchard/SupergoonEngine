﻿////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using IUpdate = SgEngine.Interfaces.IUpdate;

namespace SgEngine.GUI.Components
{
    public abstract class GuiComponent : IUpdate
    {
        /// <summary>
        /// Returns the position of the parents global plus the components local position
        /// </summary>
        public Vector2 GlobalPosition
        {
            get
            {
                if (_parent == null)
                    return _offset;
                return _parent.GlobalPosition + _offset;
            }
        }

        /// <summary>
        /// Gets the bounding box with the origin removed, this is useful for interacting with the mouse
        /// </summary>
        public RectangleF BoundingBoxAfterOrigin
        {
            get
            {
                var tempValue = GlobalPosition.ToPoint();
                tempValue -= new Point(_size.X / 2, _size.Y / 2);
                return new Rectangle(tempValue, _size);

            }
        }

        public Vector2 Origin
        {
            get
            {
                Vector2 newVector2 = new Vector2();
                newVector2.X = _size.X / 2;
                newVector2.Y = _size.Y / 2;
                return newVector2;
            }
        }

        /// <summary>
        /// Gets and sets the debug value for this component.  This is used for drawing debug boxes and logging output, etc
        /// </summary>
        public bool DebugMode
        {
            get => _debugMode;
            set => _debugMode = value;
        }

        /// <summary>
        /// Returns a bounding box for this component
        /// </summary>
        public Rectangle BoundingBox => new Rectangle(GlobalPosition.ToPoint(), _size);

        /// <summary>
        /// Gets the current objects object number
        /// </summary>
        public int UiObjectNumber => _objectNumber;

        /// <summary>
        /// Size of the current UI object
        /// </summary>
        public Point Size => _size;
        protected Point _size;
        protected int _objectNumber;
        protected Vector2 _offset = Vector2.Zero;
        protected bool IsActive;
        protected GuiComponent _parent;
        protected Enum _componentType;
        protected bool _debugMode;
        protected bool _isSelected;
        private static int _nextObjectNumber;
        /// <summary>
        /// This is an abstract class that every Ui object inherits from. 
        /// </summary>
        /// <param name="offset">The local position of this object, which could either be its offset from parent, or its actual position on the screen depending if it has a parent</param>
        /// <param name="size">The size of this object, used for bounding boxes for overlaps</param>
        /// <param name="parent">The parent of this object.  This is used in location translation, etc</param>
        protected GuiComponent(Vector2 offset, Point size = new Point(), GuiComponent parent = null)
        {
            AssignObjectNumber();
            _parent = parent;
            _offset = offset;
            _size = size;
        }

        private void AssignObjectNumber()
        {
            _objectNumber = _nextObjectNumber;
            _nextObjectNumber++;
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

        public virtual void OnSelected()
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