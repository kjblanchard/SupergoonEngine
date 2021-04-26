////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Linq.Expressions;
using Microsoft.Xna.Framework;
using SgEngine.EKS;

namespace SgEngine.Core
{
    public class SingleFunctionTimer : Timer
    {
        protected readonly Action<GameObject> _theActionToPerform;
        protected readonly GameObject _theGaeobGameObject;
        public SingleFunctionTimer(int msToWait, GameObject theGameobject, Action<GameObject> funcToUse) : base(msToWait)
        {

            _theGaeobGameObject = theGameobject;
            _theActionToPerform = funcToUse;
        }

        public override void Update(GameTime gameTime)
        {
            if (_timerCompleted)
                return;
            base.Update(gameTime);
            if (_totalMsWaited >= _msFullWaitTime)
            {
                _timerCompleted = true;
                _theActionToPerform(_theGaeobGameObject);
            }
        }
    }
    public class MultiPurposeTimer : Timer
    {
        private Action<GameObject,float> _expression;

        private GameObject gameObject;
        private float floatToUse;
        public MultiPurposeTimer(int msToWait, Action<GameObject,float> expression, GameObject go, float floatToPassIn) : base(msToWait)
        {
            _expression = expression;
            gameObject = go;
            floatToUse = floatToPassIn;

        }

        public override void Update(GameTime gameTime)
        {
            if (_timerCompleted)
                return;
            base.Update(gameTime);
            if (_totalMsWaited >= _msFullWaitTime)
            {
                _timerCompleted = true;
                _expression(gameObject, floatToUse);
            }
        }
    }
}