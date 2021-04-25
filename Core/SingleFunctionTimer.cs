////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
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

    public class SingleFunctionTimer <T>: Timer
    {
        protected readonly Action <T>_theActionToPerform;
        protected readonly T _thingDoDoItTo;
        public SingleFunctionTimer(int msToWait, T thingToDoItTo,Action <T>funcToUse) : base(msToWait)
        {

            _theActionToPerform = funcToUse;
            _thingDoDoItTo = thingToDoItTo;
        }

        public override void Update(GameTime gameTime)
        {
            if (_timerCompleted)
                return;
            base.Update(gameTime);
            if (_totalMsWaited >= _msFullWaitTime)
            {
                _timerCompleted = true;
                _theActionToPerform(_thingDoDoItTo);
            }
        }
    }
}