////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;
using SgEngine.GUI.Types;

namespace SgEngine.SgDebug
{
    /// <summary>
    /// Used to hold a string with a color to be wrote to the debug console
    /// </summary>
    public struct StringAndColor
    {
        /// <summary>
        /// What you actually want to write
        /// </summary>
        public string WhatToWrite;
        /// <summary>
        /// The color that you want to write it in
        /// </summary>
        public Color ColorToWrite;
    }
    /// <summary>
    /// The panel that actually shows in game and handles your debug commands.  This will send the commands to its subscribers
    /// </summary>
    public class SgDebugPanel : Panel
    {
        /// <summary>
        /// Is the console window open in game
        /// </summary>
        public static bool IsConsoleWindowOpen => _isConsoleWindowOpen;
        private static bool _isConsoleWindowOpen;
        private static readonly List<StringAndColor> _displayHistory = new List<StringAndColor>();
        private Rectangle DrawRectangle => new Rectangle(_debugWindowLocation, _debugWindowSize);
        private Point _debugWindowSize;
        private readonly Point _debugWindowLocation = new Point(0, 0);
        private Texture2D _backgroundTextureToDraw;
        private SpriteFont _fontToWriteWith;
        private GameWindow _gameWindow;
        private int _totalMsWaitedForCursor;
        private bool _shouldShowCursor;
        private StringBuilder _displayLine = new StringBuilder(_maxCharsInLine, _maxCharsInLine);
        private readonly StringAndColor _commandExecuted = new StringAndColor
        { ColorToWrite = Color.Green, WhatToWrite = "Command Executed" };
        private readonly StringAndColor _commandFailed = new StringAndColor
        { ColorToWrite = Color.Red, WhatToWrite = "Invalid Command" };
        private const string _terminalIndicator = "console>";
        private const int _msForEachBlink = 500;
        private const int _maxCharsInLine = 40;
        private const int _consoleTextXOffset = 5;
        private const int _consoleTextYOffset = -20;
        private const int _spaceBetweenConsoleLines = 10;




        private void TextInputHandler(object sender, TextInputEventArgs args)
        {
            HandleTilde(args);
            if (!IsConsoleWindowOpen) return;
            HandleLetterKeys(args);
            HandleSpecialKeys(args);
        }

        private static void HandleTilde(TextInputEventArgs args)
        {
            if (args.Key == Keys.OemTilde)
                _isConsoleWindowOpen = !_isConsoleWindowOpen;
        }

        private void HandleSpecialKeys(TextInputEventArgs args)
        {
            switch (args.Key)
            {
                case Keys.Back when _displayLine.Length > 0:
                    _displayLine.Remove(_displayLine.Length - 1, 1);
                    break;
                case Keys.Space:
                    _displayLine.Append(' ');
                    break;
                case Keys.Enter:
                    HandleEnter();
                    break;
                case Keys.Tab:
                    HandleTab();
                    break;
            }
        }

        private void HandleLetterKeys(TextInputEventArgs args)
        {
            if (_displayLine.Length >= _maxCharsInLine) return;
            if (char.IsLower(args.Character))
                _displayLine.Append(args.Character);
            if (char.IsUpper(args.Character))
                _displayLine.Append(char.ToLower(args.Character));
        }

        private void HandleTab()
        {
            var commandList = DebugDictionary.SearchForCommands(_displayLine.ToString());
            switch (commandList.Count)
            {
                case 0:
                    AddToDisplayHistory(_commandFailed);
                    break;
                case 1:
                    _displayLine.Clear();
                    _displayLine.Append(commandList[0]);
                    break;
                default:
                    foreach (var command in commandList)
                    {
                        AddToDisplayHistory(command, Color.White);
                    }
                    break;
            }
        }

        private void HandleEnter()
        {
            if (_displayLine.Length == 0)
                return;
            var command = DetermineCommand(_displayLine.ToString());
            switch (command)
            {
                case DebugCommands.Default:
                    AddToDisplayHistory(_displayLine.ToString(), Color.White);
                    AddToDisplayHistory(_commandFailed);
                    break;
                case DebugCommands.Exit:
                    GameWorld.ExitGame();
                    break;
                case DebugCommands.DebugMode:
                    AddToDisplayHistory(_displayLine.ToString(), Color.White);
                    AddToDisplayHistory(_commandExecuted);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            _displayLine.Clear();
        }

        private DebugCommands DetermineCommand(string potentialCommand)
        {
            var searchForFullCommand = DebugDictionary.GetDebugCommand(potentialCommand);
            if (searchForFullCommand != DebugCommands.Default)
                return searchForFullCommand;
            return TryToAutoComplete();
        }

        private DebugCommands TryToAutoComplete()
        {
            var commandList = DebugDictionary.SearchForCommands(_displayLine.ToString());
            return commandList.Count == 1 ? DebugDictionary.GetDebugCommand(commandList[0]) : DebugCommands.Default;
        }

        public override void Initialize()
        {
            base.Initialize();
            SetupWindowSize();
            SetupWindowBackground();
            _gameWindow.TextInput += TextInputHandler;
        }

        private void SetupWindowBackground()
        {
            _backgroundTextureToDraw = new Texture2D(GameWorld.GetGraphicsDevice, 1, 1);
            _backgroundTextureToDraw.SetData(new Color[] {Color.Black});
        }

        private void SetupWindowSize()
        {
            _gameWindow = GameWorld.GameWindow;
            _debugWindowSize = GameWorld.GameWorldSize;
            _debugWindowSize.Y = _debugWindowSize.Y / 2;
        }

        public override void Update(GameTime gameTime)
        {
            if (!IsConsoleWindowOpen) return;
            base.Update(gameTime);
            UpdateBlinkingCursor(gameTime);
        }

        private void UpdateBlinkingCursor(GameTime gameTime)
        {
            _totalMsWaitedForCursor += gameTime.ElapsedGameTime.Milliseconds;
            if (_totalMsWaitedForCursor >= _msForEachBlink)
            {
                _shouldShowCursor = !_shouldShowCursor;
                _totalMsWaitedForCursor = 0;
            }
        }

        public static void AddToDisplayHistory(string textToAdd, Color colorToWrite)
        {
            if (textToAdd.Length > _maxCharsInLine)
                textToAdd.Remove(_maxCharsInLine);
            _displayHistory.Add(new StringAndColor { ColorToWrite = colorToWrite, WhatToWrite = textToAdd });
            if (_displayHistory.Count > 9)
                _displayHistory.RemoveAt(0);
        }

        private void AddToDisplayHistory(StringAndColor stringAndColor)
        {
            _displayHistory.Add(stringAndColor);
        }


        public override void LoadContent()
        {
            base.LoadContent();

            _fontToWriteWith = GameWorld.FullContentManager.Load<SpriteFont>("Fonts/DebugText");
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            if (!IsConsoleWindowOpen) return;
            base.Draw(gameTime, spriteBatch);
            spriteBatch.Draw(_backgroundTextureToDraw, DrawRectangle, Color.White);
            var locationToDrawFirstLine = LocationToDrawFirstLine();
            var stringToWrite = new StringBuilder(_terminalIndicator + _displayLine);
            DrawBlinkingCursor(stringToWrite);
            spriteBatch.DrawString(_fontToWriteWith, stringToWrite, locationToDrawFirstLine.ToVector2(),
                Color.White);
            DrawHistory(spriteBatch, locationToDrawFirstLine);
        }

        private void DrawHistory(SpriteBatch spriteBatch, Point locationToDrawFirstLine)
        {
            var whatLineToDrawTo= 1;
            for (int i = _displayHistory.Count - 1; i >= 0; i--)
            {
                var wordToWrite = _displayHistory[i];
                var drawPos = locationToDrawFirstLine;
                var yOffset = _spaceBetweenConsoleLines * whatLineToDrawTo;
                drawPos.Y -= yOffset;
                spriteBatch.DrawString(_fontToWriteWith, wordToWrite.WhatToWrite, drawPos.ToVector2(),
                    wordToWrite.ColorToWrite);
                whatLineToDrawTo++;
            }
        }

        private void DrawBlinkingCursor(StringBuilder stringToWrite)
        {
            if (_shouldShowCursor)
                stringToWrite.Append('_');
        }

        private Point LocationToDrawFirstLine()
        {
            var locationToDrawFirstLine = _debugWindowSize;
            locationToDrawFirstLine.Y += _consoleTextYOffset;
            locationToDrawFirstLine.X = _consoleTextXOffset;
            return locationToDrawFirstLine;
        }
    }
}