////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;
using SgEngine.GUI.Types;

namespace SgEngine.SgDebug
{
    public struct StringAndColor
    {
        public string WhatToWrite;
        public Color ColorToWrite;

    }
    public class SgDebugPanel : Panel
    {
        public Rectangle DrawRectangle => new Rectangle(debugWindowLocation, _debugWindowSize);
        public static bool IsConsoleWindowOpen;
        private Point _debugWindowSize;
        private readonly Point debugWindowLocation = new Point(0, 0);
        private Texture2D _textureToDraw;
        private SpriteFont _fontToWriteWith;
        private const string _terminalIndicator = "console>";
        private GameWindow _gameWindow;
        private const int _msForEachBlink = 500;
        private int _totalMsWaited;
        private bool _shouldShowCursor;

        private StringBuilder _displayLine = new StringBuilder(30, 30);
        private List<StringBuilder> _displayHistory = new List<StringBuilder>();
        private int _counter = 7;
        private const string _commandExecuted = "*gCommand Executed";
        private const string _commandFailed = "*rInvalid Command";

        //private readonly List<string> _displayHistory = new List<string>();



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
                IsConsoleWindowOpen = !IsConsoleWindowOpen;
        }

        private void HandleSpecialKeys(TextInputEventArgs args)
        {
            switch (args.Key)
            {
                case Keys.Back when _displayLine.Length > 0:
                    //_displayLine = _displayLine[..^1];
                    break;
                case Keys.Space:
                    //_displayLine += " ";
                    break;
                case Keys.Enter:
                    HandleEnter();
                    break;
            }
        }

        private void HandleLetterKeys(TextInputEventArgs args)
        {
            if (char.IsLetter(args.Character))
                _displayLine.Append(args.Character);
        }

        private void HandleEnter()
        {
            if (_displayLine.Length == 0)
                return;
            switch (_displayLine.ToString())
            {
                case "debug enable":
                    AddToDisplayHistory(_displayLine.ToString());
                    AddToDisplayHistory(_commandExecuted);
                    break;
                case "exit":
                    GameWorld.ExitGame();
                    break;
                default:
                    AddToDisplayHistory(_displayLine.ToString());
                    AddToDisplayHistory(_commandFailed);
                    break;
            }

            _displayLine.Clear();
        }
        public override void Initialize()
        {
            base.Initialize();
            _gameWindow = GameWorld.GameWindow;
            _gameWindow.TextInput += TextInputHandler;
            _debugWindowSize = GameWorld.GameWorldSize;
            _debugWindowSize.Y = _debugWindowSize.Y / 2;
            _textureToDraw = new Texture2D(GameWorld.GetGraphicsDevice, 1, 1);
            _textureToDraw.SetData(new Color[] { Color.Black });
            //for (int i = 0; i < _displayHistory.Count; i++)
            //{
            //    _displayHistory[i] = new StringBuilder(30);
            //}
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            _totalMsWaited += gameTime.ElapsedGameTime.Milliseconds;
            if (_totalMsWaited >= _msForEachBlink)
            {
                _shouldShowCursor = !_shouldShowCursor;
                _totalMsWaited = 0;
            }
        }

        private void AddToDisplayHistory(string textToAdd)
        {
            //_displayHistory[_counter].Clear();
            //_displayHistory[_counter].Append(textToAdd);
            _displayHistory.Add(new StringBuilder(textToAdd));
            if (_displayHistory.Count > 9)
                _displayHistory.RemoveAt(0);

        }


        public override void LoadContent()
        {
            base.LoadContent();

            _fontToWriteWith = GameWorld.FullContentManager.Load<SpriteFont>("Fonts/DebugText");
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            if (IsConsoleWindowOpen)
            {
                spriteBatch.Draw(_textureToDraw, DrawRectangle, Color.White);
                var locationToDrawFirstLine = _debugWindowSize;
                locationToDrawFirstLine.Y -= 20;
                locationToDrawFirstLine.X = 5;
                var stringToWrite = new StringBuilder(_terminalIndicator + _displayLine);
                if (_shouldShowCursor)
                    stringToWrite.Append('_');
                spriteBatch.DrawString(_fontToWriteWith, stringToWrite, locationToDrawFirstLine.ToVector2(),
                    Color.White);
                var counter = 1;
                for (int i = _displayHistory.Count - 1; i >= 0; i--)
                {
                    var wordToWrite = _displayHistory[i];
                    var drawPos = locationToDrawFirstLine;
                    var yOffset = 10 * counter;
                    drawPos.Y -= yOffset;
                    var colorToWrite = Color.White;
                    //if (wordToWrite.ToString().Contains("*g"))
                    //{
                    //    wordToWrite.Replace("*g", "");
                    //    colorToWrite = Color.Green;
                    //}
                    //if (wordToWrite.ToString().Contains("*r"))
                    //{
                    //    wordToWrite.Replace("*r", "");
                    //    colorToWrite = Color.Red;
                    //}
                    spriteBatch.DrawString(_fontToWriteWith, wordToWrite, drawPos.ToVector2(), colorToWrite);
                    counter++;
                }

            }
        }
    }
}