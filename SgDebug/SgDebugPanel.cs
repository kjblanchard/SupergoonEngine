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
using SgEngine.Core;
using SgEngine.EKS;
using SgEngine.GUI.Types;
using SgEngine.Models;
using SgEngine.SgJson.Parsing;

namespace SgEngine.SgDebug
{
    public class SgDebugPanel : Panel
    {
        public Rectangle DrawRectangle => new Rectangle(debugWindowLocation, debugWindowSize);
        private Point debugWindowSize;
        private Point debugWindowLocation = new Point(0, 0);
        private Texture2D _textureToDraw;
        private SpriteFont _fontToWriteWith;
        private List<string> _displayHistory = new List<string>();
        private const string terminalIndicator = "console>";
        private string _displayLine = String.Empty;
        private char typedChar;
        private GameWindow gameWindow;
        private const int msForEachBlink = 500;
        private int totalMsWaited;
        private bool shouldShowCursor;
        public static bool _isConsoleWindowOpen;



        private void TextInputHandler(object sender, TextInputEventArgs args)
        {
            if (args.Key == Keys.OemTilde)
            {
                _isConsoleWindowOpen = !_isConsoleWindowOpen;
            }
            if(!_isConsoleWindowOpen) return;
            if (Char.IsLetter(args.Character))
                _displayLine += args.Character;
            if (args.Key == Keys.Back && _displayLine.Length > 0)
                _displayLine = _displayLine[..^1];
            if (args.Key == Keys.Space)
                _displayLine += " ";
            if (args.Key == Keys.Enter)
                HandleEnter();
        }

        private void HandleEnter()
        {
            if(String.IsNullOrEmpty(_displayLine))
                return;
            if (_displayLine != "debug enable")
            {
                AddToDisplayHistory(_displayLine);
                AddToDisplayHistory("*rInvalid Command");
                _displayLine = "";
            }
            else
            {
                AddToDisplayHistory(_displayLine);
                AddToDisplayHistory("*gCommand Executed");
                _displayLine = "";
            }
        }
        public override void Initialize()
        {
            base.Initialize();
            gameWindow = GameWorld.GameWindow;
            gameWindow.TextInput += TextInputHandler;
            debugWindowSize = GameWorld.GameWorldSize;
            debugWindowSize.Y = debugWindowSize.Y / 2;
            _textureToDraw = new Texture2D(GameWorld.GetGraphicsDevice, 1, 1);
            _textureToDraw.SetData(new Color[] { Color.Black });
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            totalMsWaited += gameTime.ElapsedGameTime.Milliseconds;
            if (totalMsWaited >= msForEachBlink)
            {
                shouldShowCursor = !shouldShowCursor;
                totalMsWaited = 0;
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
            if (_isConsoleWindowOpen)
            {
                spriteBatch.Draw(_textureToDraw, DrawRectangle, Color.White);
                var locationToDrawFirstLine = debugWindowSize;
                locationToDrawFirstLine.Y -= 20;
                locationToDrawFirstLine.X = 5;
                var stringToWrite = terminalIndicator + _displayLine;
                if (shouldShowCursor)
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