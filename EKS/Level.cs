using System;
using System.Collections.Generic;
using System.Text;
using SgEngine.Interfaces;
using SgEngine.State;

namespace SgEngine.EKS
{
    public class Level<T> : IUpdate where T: IState
    {

        private StateMachine<T> _stateMachine;



        protected void AddStage(Enum enumKey, T stageToAdd)
        {
            _stateMachine.AddGameState(enumKey, stageToAdd);
            
        }
    }
}
