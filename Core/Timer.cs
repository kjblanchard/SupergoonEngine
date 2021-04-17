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

        
        private int msLeftToWait;
        private Action _theActionToPerform;
        public Timer(int msTowait,Action funcToUse)
        {

            msLeftToWait = msTowait;
            _theActionToPerform = funcToUse;
        }

        public void Update(GameTime gameTime)
        {
            msLeftToWait -= gameTime.ElapsedGameTime.Milliseconds;
            if (msLeftToWait <= 0)
            {
                _theActionToPerform();
            }

        }
    }
}