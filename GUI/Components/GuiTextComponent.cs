////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended;
using MonoGame.Extended.BitmapFonts;
using SgEngine.Core;
using SgEngine.EKS;

namespace SgEngine.GUI.Components
{
    /// <summary>
    /// The configuration file used to create a new text component. 
    /// </summary>
    public class TextBoxConfig
    {
        /// <summary>
        /// The actual Text that you want displayed
        /// </summary>
        public string displayText;
        /// <summary>
        /// The parent of this text object, used for offsetting and drawing properly in the panel.
        /// </summary>
        public GuiComponent parent;
        /// <summary>
        /// The offset from the parent that you want the text.
        /// </summary>
        public Vector2 parentOffset;
        public Point textBoxSize;
        public string fontType;
        public GuiTextComponent.Alignment alignment;
        public Color textColor = Color.White;

    }
    public class GuiTextComponent : GuiComponent
    {
        //TODO move this out of the guitextcomponent file
        /// <summary>
        /// The different fonts in the game that we can use in text components, this will likely move out somewhere else
        /// </summary>
        public struct FontTypes
        {
            public const string ChronoTypeRegular = "Fonts/ChronoType";
        }

        /// <summary>
        /// The ways that we can align the text
        /// </summary>
        public enum Alignment
        {
            Center,
            Left
        }

        public Color TextColor
        {
            set => _textBoxConfig.textColor = value;
        }

        private TextBoxConfig _textBoxConfig;
        private SpriteFont _font;
        public GuiTextComponent(TextBoxConfig configuration) : base(configuration.parentOffset, configuration.textBoxSize, configuration.parent)
        {
            _textBoxConfig = configuration;
        }

        public override void Initialize()
        {
            base.Initialize();
            _font = ContentLoader.LoadFont(_textBoxConfig.fontType);
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            var drawLocation = GlobalPosition;
            var measuredText = _font.MeasureString(_textBoxConfig.displayText);
            var textOrigin = new Vector2(measuredText.X / 2, measuredText.Y / 2);
            drawLocation -= _parent.Origin;
            drawLocation = _textBoxConfig.alignment switch
            {
                Alignment.Left => AlignLeft(drawLocation, textOrigin),
                Alignment.Center => AlignCenter(measuredText, drawLocation, textOrigin),
                _ => throw new ArgumentOutOfRangeException()
            };
            spriteBatch.DrawString(_font, _textBoxConfig.displayText, drawLocation, _textBoxConfig.textColor, 0, textOrigin, 1,
                SpriteEffects.None, 1);
            if(_debugMode)
                DrawDebugBox(spriteBatch, new Rectangle(drawLocation.ToPoint(), _size), textOrigin);
        }

        private Vector2 AlignCenter(Vector2 measuredText, Vector2 drawLocation, Vector2 textOrigin)
        {
            var whitespaceSize = _size.X - measuredText.X;
            var xAlign = whitespaceSize / 2;
            drawLocation.X += textOrigin.X;
            drawLocation.Y += textOrigin.Y;
            drawLocation.X += xAlign;
            return drawLocation;
        }

        private static Vector2 AlignLeft(Vector2 drawLocation, Vector2 textOrigin)
        {
            drawLocation.X += textOrigin.X;
            drawLocation.Y += textOrigin.Y;
            return drawLocation;
        }

        /// <summary>
        /// Draws a Debug box around the text
        /// </summary>
        /// <param name="spriteBatch">The spritebatch to draw to</param>
        /// <param name="positionToDraw">The position to draw on the screen</param>
        /// <param name="textOrigin">The origin of the text, so that we can rotate it and stuff</param>
        private void DrawDebugBox(SpriteBatch spriteBatch, Rectangle positionToDraw, Vector2 textOrigin)
        {
            positionToDraw.X -= (int)textOrigin.X;
            positionToDraw.Y -= (int)textOrigin.Y;
            spriteBatch.DrawRectangle(positionToDraw, Color.Red);
        }
    }
}