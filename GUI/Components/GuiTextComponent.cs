////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using MonoGame.Extended.BitmapFonts;
using SgEngine.Core;
using SgEngine.EKS;

namespace SgEngine.GUI.Components
{
    public class TextBoxConfig
    {
        public string displayText;
        public GuiComponent parent;
        public Vector2 parentOffset;
        public Point textBoxSize;
        public string fontType;
        public GuiTextComponent.Alignment alignment;
        
    }
    public class GuiTextComponent : GuiComponent
    {
        public struct FontTypes
        {
            public const string ChronoTypeRegular = "Fonts/ChronoType";
        }
        public enum Alignment { Center, Left, Right, Top, Bottom }

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
            drawLocation -= _parent.Origin;
            var measuredText = _font.MeasureString(_textBoxConfig.displayText);
            var textOrigin = new Vector2(measuredText.X / 2, measuredText.Y / 2);
             switch (_textBoxConfig.alignment)
             {
                 case Alignment.Left:
                     drawLocation.X += textOrigin.X;
                     drawLocation.Y += textOrigin.Y;
                     break;
                 case Alignment.Center:
                     break;
                 case Alignment.Right:
                     break;
                 case Alignment.Top:
                     break;
                 case Alignment.Bottom:
                     break;
                 default:
                     throw new ArgumentOutOfRangeException();
             }
            //spriteBatch.DrawString(_font,_textBoxConfig.displayText,drawLocation,Color.Yellow);
            spriteBatch.DrawString(_font, _textBoxConfig.displayText, drawLocation, Color.White, 0, textOrigin, 1,
                SpriteEffects.None, 1);
        }
    }
}