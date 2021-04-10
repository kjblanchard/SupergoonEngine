using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Media;
using SgEngine.Models;

namespace SgEngine.Core.Sounds
{
    public class Sound
    {
        private  Song _currentPlayingSong;
        private  Introloop _currentPlayingIntroloop;
        private readonly Dictionary<int,Introloop> _allIntroLoopMusic = new Dictionary<int, Introloop>();
        private readonly ContentManager _contentManager;

        public Sound(ContentManager contentManager)
        {
            _contentManager = contentManager;

        }

        public  void Startup()
        {
            //using var streamReader = new StreamReader("Config/Sound/" + "music" + ".json");
            using var streamReader = Utils.Utils.CreateJsonStreamReader("music",Utils.Utils.ConfigFileToRead.MusicFile);

            var fileContents = streamReader.ReadToEnd();
            var loadedIntroloopsJson = System.Text.Json.JsonSerializer.Deserialize<IntroLoops>(fileContents);
            foreach (var introLoop in loadedIntroloopsJson.Introloops)
            {
                _allIntroLoopMusic.Add(
                    introLoop.Id,
                    new Introloop(
                        introLoop.Id,
                        introLoop.SongName,
                        new TimeSpan(introLoop.LoopStartTime[0],introLoop.LoopStartTime[1],introLoop.LoopStartTime[2],introLoop.LoopStartTime[3],introLoop.LoopStartTime[4]),
                        new TimeSpan(introLoop.LoopEndTime[0],introLoop.LoopEndTime[1],introLoop.LoopEndTime[2],introLoop.LoopEndTime[3],introLoop.LoopEndTime[4])
                    )
                );
            }
        }

        public  void PlayMusic(Enum enumValue)
        {
            var value = Convert.ToInt32(enumValue);
            _currentPlayingIntroloop = _allIntroLoopMusic[value];
            if(_currentPlayingIntroloop == null)
                return;
            _currentPlayingSong = _contentManager.Load<Song>(_currentPlayingIntroloop.SongName);   
            MediaPlayer.Play(_currentPlayingSong);
        }

        public  void Update()
        {
            var currentPos = MediaPlayer.PlayPosition;
            if (currentPos >= _currentPlayingIntroloop.LoopEndTime)
            {
                MediaPlayer.Play(_currentPlayingSong,_currentPlayingIntroloop.LoopStartTime);
            }
            
        }
    }

}
