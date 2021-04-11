using System;
using System.Collections.Generic;

namespace SgEngine.Models
{
    public class Bankfile
    {
        public string BankDescription { get; set; }
        public string BankName { get; set; }
        public string BankStringName { get; set; }
        public List<Event> Events { get; set; }

    }


    public class Event
    {
        public string EventName { get; set; }
        public int SongId { get; set; }
    }

    public class BankfileList
    {
        public List<Bankfile> Bankfiles { get; set; }
    }
}
