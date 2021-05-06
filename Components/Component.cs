////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.EKS;
using SgEngine.Interfaces;

namespace SgEngine.Components
{
    public class Component : IUpdate
    {
        public Vector2 GlobalPosition
        {
            get => _parent.GlobalPosition + _localPosition;
        }
        protected bool _isActive = true;
        protected GameObject _parent;
        protected Vector2 _localPosition;
        protected Vector2 _ownerOffset;
        public Enum _componentType;

        public Component(GameObject parent)
        {
            _parent = parent;
        }

        public virtual void Initialize(){}
        public virtual void LoadContent(){}
        public virtual void BeginRun(){}
        public virtual void Update(GameTime gameTime){}
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch) {}
        public virtual void OnActivate(){}
        public virtual void OnDeactivate(){}
    }
}