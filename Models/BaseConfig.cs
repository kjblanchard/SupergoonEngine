using System;
using System.Collections.Generic;
using System.Text;

namespace SgEngine.Models
{
   public class BaseConfig
    {
        public Window Window { get; set; }
        public World World { get; set; }

    }
    public class Window
    {
        public int X { get; set; }
        public int Y { get; set; }
    }

    public class World
    {
        public int X { get; set; }
        public int Y { get; set; }
    }
}
