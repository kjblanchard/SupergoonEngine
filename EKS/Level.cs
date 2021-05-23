using System;
using System.Collections.Generic;
using System.Text;
using SgEngine.Interfaces;
using SgEngine.Interfaces.EKS;
using SgEngine.State;

namespace SgEngine.EKS
{
    public class Level<T> : IFullEksObject where T: IState
    {

        private StateMachine<T> _stateMachine;



        protected void AddStage(Enum enumKey, T stageToAdd)
        {
            _stateMachine.AddGameState(enumKey, stageToAdd);
            
        }
    }
}
