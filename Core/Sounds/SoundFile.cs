using System;
using System.Collections.Generic;
using System.Text;
using FMOD.Studio;
using SgEngine.Models;

namespace SgEngine.Core.Sounds
{
    class SoundFile
    {
        public FMOD.Studio.Bank[] Banks { get; set; } = new Bank[2];

        public Dictionary<int, EventFile> EventDescriptions { get; set; } =
            new Dictionary<int, EventFile>();

        public SoundFile(Bankfile bankFileToConvert, FMOD.Studio.System systemToLoadFrom)
        {
            systemToLoadFrom.loadBankFile(bankFileToConvert.BankName, FMOD.Studio.LOAD_BANK_FLAGS.NORMAL,
                out var mainBank);
            systemToLoadFrom.loadBankFile(bankFileToConvert.BankStringName, FMOD.Studio.LOAD_BANK_FLAGS.NORMAL,
                out var stringBank);
            Banks[0] = mainBank;
            Banks[1] = stringBank;

            foreach (var @event in bankFileToConvert.Events)
            {
                systemToLoadFrom.getEvent($"event:/{@event.EventName}", out var tempDescription);
                EventDescriptions.Add(@event.SongId, new EventFile(
                    tempDescription, new EventInstance(), @event.EventName));
            }

        }


    }



    class EventFile
    {
        public EventDescription Description { get; }
        public EventInstance Instance { get; set; }
        public string SongName { get; }

        public EventFile(EventDescription description, EventInstance instance, string songName)
        {
            Description = description;
            Instance = instance;
            SongName = songName;
        }

    }
}
