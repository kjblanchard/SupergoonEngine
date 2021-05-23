////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;

namespace SgEngine.Models.Ui
{

    public class Location
    {
        public int X { get; set; }
        public int Y { get; set; }
    }

    public class Size
    {
        public int X { get; set; }
        public int Y { get; set; }
    }

    public class ParentOffset
    {
        public int X { get; set; }
        public int Y { get; set; }
    }

    public class TextComponent
    {
        public string Alignment { get; set; }
        public string DisplayText { get; set; }
        public int FontType { get; set; }
        public ParentOffset ParentOffset { get; set; }
    }

    public class Button
    {
        public string Name { get; set; }
        public int Type { get; set; }
        public Location Location { get; set; }
        public Size Size { get; set; }
        public int Graphic { get; set; }
        public TextComponent TextComponent { get; set; }
    }

    public class ButtonController
    {
        public int Type { get; set; }
        public List<Button> Buttons { get; set; }
    }

    public class Panel
    {
        public string Name { get; set; }
        public Location Location { get; set; }
        public Size Size { get; set; }
        public int Graphic { get; set; }
        public List<TextComponent> TextComponents { get; set; }
        public List<Button> Buttons { get; set; }
        public List<ButtonController> ButtonControllers { get; set; }
    }

    public class UiScreenModel
    {
        public List<Panel> Panels { get; set; }
    }


}