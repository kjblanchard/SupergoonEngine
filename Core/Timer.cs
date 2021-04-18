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
    public class Timer
    {


        protected int _totalMsWaited = 0;
        protected int _msStartingTime;
        protected readonly Action _theActionToPerform;
        protected readonly Action<float> _theOtherActiontoPerform;

        private bool _timerCompleted = false;
        //TODO move this implementation as abstract, and then make two timers
        public Timer(int msTowait, Action funcToUse)
        {
            _msStartingTime = msTowait;
            _theActionToPerform = funcToUse;
        }
        public Timer(int msTowait, Action<float> funcToUse)
        {
            _msStartingTime = msTowait;
            _theOtherActiontoPerform = funcToUse;
        }




        public virtual void Update(GameTime gameTime)
        {
            if (!_timerCompleted)
                _totalMsWaited += gameTime.ElapsedGameTime.Milliseconds;
            if (_totalMsWaited >= _msStartingTime)
            {
                _theActionToPerform();
            }
        }

    }
}