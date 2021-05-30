////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using SgEngine.Core.Input;
using SgEngine.EKS;

namespace SgEngine.Interfaces.EKS
{
    public interface IHandlePlayerInput
    {

        public List<ControllerButtonAndAction> ThisFrameControllerButtonAndActions { get; set; }

        public void ButtonPressedEventHandler(object sender, List<ControllerButtonAndAction> actionsThisFrame)

        {
            //TODO Figure out a better way than this, since this just sets a reference to this list and now actually populates a list
            ThisFrameControllerButtonAndActions = actionsThisFrame;
        }
        public void TakeControl(int playerNumber)
        {

            GameWorld.GetPlayerController(playerNumber).OnButtonsPressed += ButtonPressedEventHandler;
        }
    }
}