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
        private readonly Action<float> _afterFunctionFloat;
       public TweenTimer(float startingValue, float endingValue, int msToWait, Action<float> funcToUse, Action afterFunc = null) : base(msToWait)
       {
            _startingValue = startingValue;
            _endingValue = endingValue;
            _diff = _endingValue - _startingValue;
            _afterFunction = afterFunc;
            _afterFunctionFloat = funcToUse;
       }



        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            if(_timerCompleted)
                return;
            if (_totalMsWaited > _msFullWaitTime)

            {
                _timerCompleted = true;
                _afterFunction?.Invoke();
                return;
            }
            _afterFunctionFloat(CalculateValue());
        }

        private float CalculateValue()
        {

            if (_totalMsWaited == 0)
                return 0;
            var percentageOfProgress = _totalMsWaited  *1.0 / _msFullWaitTime;
            var progressAddition = _diff * percentageOfProgress;
            return (float)(_startingValue + progressAddition);
        }

    }
}