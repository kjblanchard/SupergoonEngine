using System;
using FMOD.Studio;
using SgEngine.Models;
using SgEngine.SgJson;

namespace SgEngine.Core.Sounds
{
    /// <summary>
    /// The FMOD sound system that SG engine uses
    /// </summary>
    public class SoundSystem
    {
        private FMOD.Studio.System _fmodStudioSystem;
        private readonly SoundFile[] _soundFileArray = new SoundFile[2];
        private EventFile _currentPlayingInstance;
        private EventFile _previousPlayingInstance;

        /// <summary>
        /// Initializes FMOD and loads the music banks
        /// </summary>
        public void Initialize()
        {
            InitializeFmodStudio();
            var deserializedBanks = ReadMusicJsonFile("music");

            for (int i = 0; i < deserializedBanks.Bankfiles.Count; i++)
            {
                var bankFileModel = deserializedBanks.Bankfiles[i];
                var soundFile = new SoundFile(bankFileModel, _fmodStudioSystem);
                _soundFileArray[i] = soundFile;
            }

        }


        /// <summary>
        /// Creates the fmod studio system, and initializes the system
        /// </summary>
        private void InitializeFmodStudio()
        {
            FMOD.Studio.System.create(out _fmodStudioSystem);
            _fmodStudioSystem.initialize(512, FMOD.Studio.INITFLAGS.NORMAL, FMOD.INITFLAGS.NORMAL, IntPtr.Zero);
        }

        /// <summary>
        /// Reads the music json file to get the sound banks
        /// </summary>
        /// <param name="fileName">This is the filename that shoud be searched for</param>
        /// <returns>Returns all of the banks in a bankfilelist</returns>
        BankfileList ReadMusicJsonFile(string fileName)
        {
            using var streamreader =
                GameJsonLoader.CreateJsonStreamReader(fileName, GameJsonLoader.ConfigFileToRead.MusicFile);
            try
            {
                var data = streamreader.ReadToEnd();
                return System.Text.Json.JsonSerializer.Deserialize<BankfileList>(data);
            }
            catch
            {
                //TODO Create a debug logger that handles these things and writes it to a file
                return null;
            }
        }

        /// <summary>
        /// This converts the enum to a value, and then plays that song from the banklist.  You should probably use a more specific enum and abstract from this to not allow
        /// for this to be taking an y enum
        /// </summary>
        /// <param name="enumValue">This is the bankfile number that will be played from the loaded sound files</param>
        public void PlayBgm(Enum enumValue)
        {
            if (_currentPlayingInstance != null)
            {
                _currentPlayingInstance.Instance.stop(STOP_MODE.ALLOWFADEOUT);
                _previousPlayingInstance = _currentPlayingInstance;
                
            }
            var value = Convert.ToInt32(enumValue);
            var chosenSoundFile = _soundFileArray[0].EventDescriptions[value];
            chosenSoundFile.Description.loadSampleData();
            chosenSoundFile.Description.createInstance(out var tempInstance);
            chosenSoundFile.Instance = tempInstance;
            chosenSoundFile.Instance.start();
            _currentPlayingInstance = chosenSoundFile;
            _previousPlayingInstance?.Instance.release();
        }

        public void PlaySfx(Enum enumValue)
        {
            var value = Convert.ToInt32(enumValue);
            var chosenSoundFile = _soundFileArray[1].EventDescriptions[value];
            chosenSoundFile.Description.loadSampleData();
            chosenSoundFile.Description.createInstance(out var tempInstance);
            chosenSoundFile.Instance = tempInstance;
            chosenSoundFile.Instance.start();
        }

        /// <summary>
        /// This method must be called every update so that fmod can function properly
        /// </summary>
        public void Update()
        {
            _fmodStudioSystem.update();
        }
    }

}
