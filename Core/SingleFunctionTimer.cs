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

        public override void Update(GameTime gameTime)
        {
            if (_timerCompleted)
                return;
            base.Update(gameTime);
            if (_totalMsWaited >= _msFullWaitTime)
            {
                _timerCompleted = true;
                _theActionToPerform();
            }
        }
    }
}