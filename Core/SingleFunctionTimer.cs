////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;

namespace SgEngine.Core
{
    public class SingleFunctionTimer : Timer
    {
        protected readonly Action _theActionToPerform;
        public SingleFunctionTimer(int msToWait, Action funcToUse) : base(msToWait)
        {

            _theActionToPerform = funcToUse;
        }

        public virtual void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            if(_timerCompleted)
                return;
            if (_totalMsWaited >= _msFullWaitTime)
            {
                _timerCompleted = true;
                _theActionToPerform();
            }
        }
    }
}