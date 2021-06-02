////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using MonoGame.Extended;
using SgEngine.Core.Input;
using SgEngine.EKS;

namespace SgEngine.Interfaces.Input
{
    public interface ISubscribeToButton
    {
        public void SubScribeToButtons();

        public void SubscribeToButtonPressed(int playerNumber, ControllerButtons buttonToSubscribeTo,
            Controller.ButtonPressedEventHandler eventHandler)
        {
            var playerController = GameWorld.GetPlayerController(playerNumber);
            switch (buttonToSubscribeTo)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    playerController.OnUpButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Right:
                    playerController.OnRightButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Down:
                    playerController.OnDownButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Left:
                    playerController.OnLeftButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Y:
                    playerController.OnYButtonPressed += eventHandler;
                    break;
                case ControllerButtons.B:
                    playerController.OnBButtonPressed += eventHandler;
                    break;
                case ControllerButtons.A:
                    playerController.OnAButtonPressed += eventHandler;
                    break;
                case ControllerButtons.X:
                    playerController.OnXButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Start:
                    playerController.OnStartButtonPressed += eventHandler;
                    break;
                case ControllerButtons.Select:
                    playerController.OnSelectButtonPressed += eventHandler;
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonToSubscribeTo), buttonToSubscribeTo, null);
            }
        }
        public void UnsubscribeToButtonPressed(int playerNumber, ControllerButtons buttonToSubscribeTo,
            Controller.ButtonPressedEventHandler eventHandler)
        {
            var playerController = GameWorld.GetPlayerController(playerNumber);
            switch (buttonToSubscribeTo)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    playerController.OnUpButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Right:
                    playerController.OnRightButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Down:
                    playerController.OnDownButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Left:
                    playerController.OnLeftButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Y:
                    playerController.OnYButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.B:
                    playerController.OnBButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.A:
                    playerController.OnAButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.X:
                    playerController.OnXButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Start:
                    playerController.OnStartButtonPressed -= eventHandler;
                    break;
                case ControllerButtons.Select:
                    playerController.OnSelectButtonPressed -= eventHandler;
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonToSubscribeTo), buttonToSubscribeTo, null);
            }
        }

        public void SubscribeToButtonHeld(int playerNumber, ControllerButtons buttonToSubscribeTo,
            Controller.ButtonPressedEventHandler eventHandler)
        {
            var playerController = GameWorld.GetPlayerController(playerNumber);
            switch (buttonToSubscribeTo)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    playerController.OnUpButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Right:
                    playerController.OnRightButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Down:
                    playerController.OnDownButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Left:
                    playerController.OnLeftButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Y:
                    playerController.OnYButtonHeld += eventHandler;
                    break;
                case ControllerButtons.B:
                    playerController.OnBButtonHeld += eventHandler;
                    break;
                case ControllerButtons.A:
                    playerController.OnAButtonHeld += eventHandler;
                    break;
                case ControllerButtons.X:
                    playerController.OnXButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Start:
                    playerController.OnStartButtonHeld += eventHandler;
                    break;
                case ControllerButtons.Select:
                    playerController.OnSelectButtonHeld += eventHandler;
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonToSubscribeTo), buttonToSubscribeTo, null);
            }
        }

        public void SubscribeToButtonReleased(int playerNumber, ControllerButtons buttonToSubscribeTo,
            Controller.ButtonPressedEventHandler eventHandler)
        {
            var playerController = GameWorld.GetPlayerController(playerNumber);
            switch (buttonToSubscribeTo)
            {
                case ControllerButtons.Default:
                    break;
                case ControllerButtons.Up:
                    playerController.OnUpButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Right:
                    playerController.OnRightButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Down:
                    playerController.OnDownButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Left:
                    playerController.OnLeftButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Y:
                    playerController.OnYButtonReleased += eventHandler;
                    break;
                case ControllerButtons.B:
                    playerController.OnBButtonReleased += eventHandler;
                    break;
                case ControllerButtons.A:
                    playerController.OnAButtonReleased += eventHandler;
                    break;
                case ControllerButtons.X:
                    playerController.OnXButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Start:
                    playerController.OnStartButtonReleased += eventHandler;
                    break;
                case ControllerButtons.Select:
                    playerController.OnSelectButtonReleased += eventHandler;
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(buttonToSubscribeTo), buttonToSubscribeTo, null);
            }

        }

        public interface IHandleAButtonPressed : ISubscribeToButton
        {
            public void AButtonEventHandler(object sender, ControllerButtons buttonPressed,
                ButtonActions actionPerformed);

            public void SubscribeToAButtonPressed(int playerNumber)
            {
                SubscribeToButtonPressed(playerNumber, ControllerButtons.A, AButtonEventHandler);
            }
        }
        public interface IHandleUpButtonPressed : ISubscribeToButton
        {
            public void UpButtonEventHandler(object sender, ControllerButtons buttonPressed,
                ButtonActions actionPerformed);

            public void SubscribeToAButtonPressed(int playerNumber)
            {
                SubscribeToButtonPressed(playerNumber, ControllerButtons.Up, UpButtonEventHandler);
            }
        }
        public interface IHandleDownButtonPressed : ISubscribeToButton
        {
            public void DownButtonEventHandler(object sender, ControllerButtons buttonPressed,
                ButtonActions actionPerformed);

            public void SubscribeToAButtonPressed(int playerNumber)
            {
                SubscribeToButtonPressed(playerNumber, ControllerButtons.A, DownButtonEventHandler);
            }
        }



    }
}