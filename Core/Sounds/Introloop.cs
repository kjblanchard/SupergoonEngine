using System;
using System.Collections.Generic;
using System.Text;

namespace SgEngine.Core.Sounds
{
    class Introloop
    {
        public int Id;
        public  TimeSpan LoopStartTime;
        public TimeSpan LoopEndTime;
        public string SongName;

        public Introloop(int id, string songName, TimeSpan loopStartTime, TimeSpan loopEndTime)
        {
            Id = id;
            SongName = songName;
            LoopStartTime = loopStartTime;
            LoopEndTime = loopEndTime;
        }
    }
}
