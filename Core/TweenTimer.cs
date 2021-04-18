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
    public class TweenTimer : Timer
    {
        private readonly float _startingValue;
        private readonly float _endingValue;
        private readonly float _diff;
        private readonly Action _afterFunction;
       public TweenTimer(float startingValue, float endingValue, int msTowait, Action<float> funcToUse, Action afterFunc = null) : base(msTowait, funcToUse)
        {
            _startingValue = startingValue;
            _endingValue = endingValue;
            _diff = _endingValue - _startingValue;
            _afterFunction = afterFunc;
        }



        public override void Update(GameTime gameTime)
        {
            if(_timerCompleted)
                return;
            if (_totalMsWaited > _msStartingTime) 
            {
                _timerCompleted = true;
                _afterFunction?.Invoke();
                return;
            }
            _totalMsWaited = gameTime.ElapsedGameTime.Milliseconds + _totalMsWaited;
            _theOtherActiontoPerform(calculateValue());
        }

        private float calculateValue()
        {

            if (_totalMsWaited == 0)
                return 0;
            var percentageOfProgress = _totalMsWaited  *1.0 / _msStartingTime;
            var progressAddition = _diff * percentageOfProgress;
            return (float)(_startingValue + progressAddition);
        }

    }
}