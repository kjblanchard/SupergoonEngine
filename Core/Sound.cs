using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Media;

namespace SgEngine.Core
{
    public class Sound
    {
        private const int SecondsBeforeLoop = 10;
        private static TimeSpan _loopTimespan;
        private static Song _currentPlayingSong;
        private static ContentManager _contentManager;
        public Sound(ContentManager contentManager)
        {
            _contentManager = contentManager;
            _loopTimespan = new TimeSpan(0,0,5);
        }

        public static void PlayMusic(string assetName, bool repeat = true)
        {
            _currentPlayingSong = _contentManager.Load<Song>(assetName);   
            MediaPlayer.Play(_currentPlayingSong);
        }

        public static void Update(GameTime gameTime)
        {
            var currentPos = MediaPlayer.PlayPosition;
            if (currentPos.Seconds > SecondsBeforeLoop)
            {
                MediaPlayer.Play(_currentPlayingSong,_loopTimespan);
            }
            
        }
    }
}
