using System;
using System.Collections.Generic;
using System.Text;

namespace SgEngine.Models
{
    public class Introloop
    {
        public int Id { get; set; }
        public string SongName { get; set; }
        public List<int> LoopStartTime { get; set; }
        public List<int> LoopEndTime { get; set; }
    }

    public class IntroLoops
    {
        public List<Introloop> Introloops { get; set; }
    }
}
