////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using SgEngine.Core.Sounds;
using SgEngine.EKS;

namespace SgEngine.Interfaces.Sound
{
    public interface IPlayMusic
    {
        private static SoundSystem _soundSystem;

        public void PlayMusic(Enum musicToPlay)
        {
            if (_soundSystem == null)
                GetSoundSystem();
            _soundSystem?.PlayBgm(musicToPlay);
        }
        private void GetSoundSystem()
        {
            _soundSystem = GameWorld.SoundSystem;
        }
    }
}