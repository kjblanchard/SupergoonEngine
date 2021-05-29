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
    public interface IPlaySfx
    {
        private static SoundSystem _soundSystem;

        public void PlaySfx(Enum enumToPlay)
        {
            if (_soundSystem == null)
                GetSoundSystem();
            _soundSystem?.PlaySfx(enumToPlay);
        }

        private void GetSoundSystem()
        {

            _soundSystem = GameWorld.SoundSystem;
        }
    }
}