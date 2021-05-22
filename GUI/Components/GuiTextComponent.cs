////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core;
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
        public string DisplayText;
        /// <summary>
        /// The parent of this text object, used for offsetting and drawing properly in the panel.
        /// </summary>
        public GuiComponent Parent;
        /// <summary>
        /// The offset from the parent that you want the text.
        /// </summary>
        public Vector2 ParentOffset;
        public Point TextBoxSize;
        public int FontType;
        public GuiTextComponent.Alignment Alignment;
        public Color TextColor = Color.White;

    }
    public class GuiTextComponent : GuiComponent
    {
        ////TODO move this out of the guitextcomponent file
        ///// <summary>
        ///// The different fonts in the game that we can use in text components, this will likely move out somewhere else
        ///// </summary>
        //public struct FontTypes
        //{
        //    public const string ChronoTypeRegular = "Fonts/ChronoType";
        //}


        /// <summary>
        /// The ways that we can align the text
        /// </summary>
        public enum Alignment
        {
            Center,
            Left
        }

        public string DisplayText => _textBoxConfig.DisplayText;

        public Color TextColor
        {
            set => _textBoxConfig.TextColor = value;
        }
        public Vector2 TextSize => _fontSheet.SpriteFont.MeasureString(_textBoxConfig.DisplayText);

        private readonly TextBoxConfig _textBoxConfig;

        private FontSheet _fontSheet;
        public GuiTextComponent(TextBoxConfig configuration, Enum fontToLoad) : base(configuration.ParentOffset, configuration.TextBoxSize, configuration.Parent)
        {
            _textBoxConfig = configuration;
            var parsedInt = Convert.ToInt32(fontToLoad);
            _fontSheet = ContentLoader.GetFontSheet(parsedInt);
        }
        public GuiTextComponent(TextBoxConfig configuration, int fontToLoad) : base(configuration.ParentOffset, configuration.TextBoxSize, configuration.Parent)
        {
            _textBoxConfig = configuration;
            _fontSheet = ContentLoader.GetFontSheet(fontToLoad);
        }

        public override void Initialize()
        {
            base.Initialize();
            AutoSetSize();
        }

        public override void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            base.Draw(gameTime, spriteBatch);
            var drawLocation = GlobalPosition;
            var measuredText = _fontSheet.SpriteFont.MeasureString(_textBoxConfig.DisplayText);
            var textOrigin = new Vector2(measuredText.X / 2, measuredText.Y / 2);
            drawLocation -= _parent.Origin;
            drawLocation = _textBoxConfig.Alignment switch
            {
                Alignment.Left => AlignLeft(drawLocation, textOrigin),
                Alignment.Center => AlignCenter(measuredText, drawLocation, textOrigin),
                _ => throw new ArgumentOutOfRangeException()
            };
            spriteBatch.DrawString(_fontSheet.SpriteFont, _textBoxConfig.DisplayText, drawLocation, _textBoxConfig.TextColor, 0, textOrigin, 1,
                SpriteEffects.None, 1);
            DrawDebugBox(spriteBatch, new Rectangle(drawLocation.ToPoint(), _size), textOrigin, Color.White);
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
        /// Sets the size of the textbox based on the size of the text
        /// </summary>
        private void AutoSetSize()
        {
            var newSize = TextSize;
            _size = new Point((int)newSize.X, (int)newSize.Y);
        }
    }
}