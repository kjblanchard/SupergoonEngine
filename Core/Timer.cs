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
    public abstract class Timer
    {


        protected int _totalMsWaited = 0;
        protected int _msFullWaitTime;
        protected bool _timerCompleted;

        protected Timer(int msToWait)
        {

            _msFullWaitTime = msToWait;
        }

        /// <summary>
        /// This should be called in the gameobjects update timers section so that time timers actually tick
        /// This base update will update the actual time, and return if the timer is completed.
        /// </summary>
        /// <param name="gametime"></param>
        public virtual void Update(GameTime gametime)
        {

            if(_timerCompleted)
                return;
            _totalMsWaited += gametime.ElapsedGameTime.Milliseconds;
        }


    }
}