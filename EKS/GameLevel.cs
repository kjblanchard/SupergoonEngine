using System;
using System.Collections.Generic;
using System.Text;
using SgEngine.Interfaces;

namespace SgEngine.EKS
{
    public class GameLevel : IUpdate
    {

        public static GameLevel _instance;

        public GameLevel()
        {
            _instance ??= this;
        }

        public static GameLevel GetLevel()
        {
            return _instance;
        }
    }
}
