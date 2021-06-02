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
using SgEngine.Interfaces.EKS;

namespace SgEngine.GUI.Components
{
    public abstract class GuiComponent : IFullEksObject
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
        /// Used when you need to position something on a specific spot on the screen and not use parents and local position to draw
        /// </summary>
        public bool LocationOverride = false;
        /// <summary>
        /// Gets the "local position" which is just this components offset, not it's parents
        /// </summary>
        public Vector2 LocalPosition
        {
            get => _offset;
            set => _offset = value;
        }
        /// <summary>
        /// Modifies the isSelected field, and calls the onselected or ondeselected m ethod
        /// </summary>
        public bool IsSelected
        {
            get => _isSelected;
            set
            {
                switch (value)
                {
                    case true:
                        OnSelected();
                        break;
                    case false:
                        OnDeselected();
                        break;
                }
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
        /// <summary>
        /// Gets the origin of all parents, useful for nested components to get the origin
        /// </summary>
        public Vector2 FullOrigin
        {
            get
            {
                if (_parent != null)
                    return _parent.FullOrigin + Origin;
                return Origin;
            }
        }
        /// <summary>
        /// Gets the origin of this object only, doesn't take into consideration parents.
        /// </summary>
        public Vector2 Origin
        {
            get
            {
                var newVector2 = new Vector2 {X = _size.X / 2, Y = _size.Y / 2};
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
        /// The components name, not really used yet
        /// </summary>
        public string ComponentName
        {
            get => _componentName;
            set => _componentName = value;
        }
        /// <summary>
        /// Size of the current UI object
        /// </summary>
        public Point Size => _size;
        protected Point _size;
        protected int _objectNumber;
        protected Vector2 _offset;
        protected bool _isActive;
        protected GuiComponent _parent;
        protected Enum _componentType;
        protected bool _debugMode;
        protected bool _isSelected;
        protected string _componentName;
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
            Gui.AllGuiComponents.Add(this);
        }
        /// <summary>
        /// Assigns this object with a number and then increments it
        /// </summary>
        private void AssignObjectNumber()
        {
            _objectNumber = _nextObjectNumber++;
        }

        public virtual void Initialize()
        {
        }

        public virtual void LoadContent()
        {
        }

        public virtual void BeginRun()
        {
            Activate();
        }

        public virtual void Update(GameTime gameTime)
        {
        }

        public virtual void HandleMouse()
        {
        }

        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
        }
        public void Activate()
        {
            OnActivate();
        }

        public void Deactivate()
        {
            OnDeactivate();
        }

        /// <summary>
        /// What happens when this component has been selected
        /// </summary>
        protected virtual void OnSelected()
        {
            _isSelected = true;

        }
        /// <summary>
        /// What happens when this component has been deselected
        /// </summary>
        protected virtual void OnDeselected()
        {
            _isSelected = false;
        }
        /// <summary>
        /// What happens when this component is activated
        /// </summary>
        protected virtual void OnActivate()
        {
            _isActive = true;
        }

        /// <summary>
        /// What happens when this component is deactivated
        /// </summary>
        protected virtual void OnDeactivate()
        {
            _isActive = false;
        }
        /// <summary>
        /// Draws the debug box around this object.  You can offset it (for graphics) so that it is drawn in the right spot
        /// </summary>
        /// <param name="spriteBatch">The spritebatch to write to</param>
        /// <param name="positionToDraw">The position on the screen to draw</param>
        /// <param name="originToOffset">The amount to subtract for offsetting</param>
        /// <param name="colorToDraw">The color to draw the box</param>
        protected void DrawDebugBox(SpriteBatch spriteBatch, RectangleF positionToDraw, Vector2 originToOffset, Color colorToDraw)
        {
            if (!DebugMode)
                return;
            positionToDraw.X -= (int)originToOffset.X;
            positionToDraw.Y -= (int)originToOffset.Y;
            spriteBatch.DrawRectangle(positionToDraw, colorToDraw);
        }

    }
}