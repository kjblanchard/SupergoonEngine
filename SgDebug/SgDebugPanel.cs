////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using SgEngine.EKS;
using SgEngine.GUI.Types;

namespace SgEngine.SgDebug
{
    public class SgDebugPanel : Panel
    {
        public Rectangle DrawRectangle => new Rectangle(debugWindowLocation, _debugWindowSize);
        public static bool IsConsoleWindowOpen;
        private Point _debugWindowSize;
        private readonly Point debugWindowLocation = new Point(0, 0);
        private Texture2D _textureToDraw;
        private SpriteFont _fontToWriteWith;
        private readonly List<string> _displayHistory = new List<string>();
        private const string _terminalIndicator = "console>";
        private string _displayLine = String.Empty;
        private GameWindow _gameWindow;
        private const int _msForEachBlink = 500;
        private int _totalMsWaited;
        private bool _shouldShowCursor;




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
                    _displayLine = _displayLine[..^1];
                    break;
                case Keys.Space:
                    _displayLine += " ";
                    break;
                case Keys.Enter:
                    HandleEnter();
                    break;
            }
        }

        private void HandleLetterKeys(TextInputEventArgs args)
        {
            if (char.IsLetter(args.Character))
                _displayLine += args.Character;
        }

        private void HandleEnter()
        {
            if (string.IsNullOrEmpty(_displayLine))
                return;
            switch (_displayLine)
            {
                case "debug enable":
                    AddToDisplayHistory(_displayLine);
                    AddToDisplayHistory("*gCommand Executed");
                    _displayLine = "";
                    break;
                case "exit":
                    GameWorld.ExitGame();
                    break;
                default:
                    AddToDisplayHistory(_displayLine);
                    AddToDisplayHistory("*rInvalid Command");
                    _displayLine = "";
                    break;
            }
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
            _displayHistory.Add(textToAdd);
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
                var stringToWrite = _terminalIndicator + _displayLine;
                if (_shouldShowCursor)
                    stringToWrite += "_";
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
                    if (_displayHistory[i].StartsWith("*r"))
                    {
                        colorToWrite = Color.Red;
                        wordToWrite = wordToWrite.Split('r')[1];
                    }
                    if (_displayHistory[i].StartsWith("*g"))
                    {
                        colorToWrite = Color.Green;
                        wordToWrite = wordToWrite.Split('g')[1];
                    }
                    spriteBatch.DrawString(_fontToWriteWith, wordToWrite, drawPos.ToVector2(), colorToWrite);
                    counter++;
                }

            }
        }
    }
}