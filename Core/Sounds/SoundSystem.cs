using System;
using SgEngine.Models;

namespace SgEngine.Core.Sounds
{
    public class SoundSystem
    {
        private FMOD.Studio.System _fmodStudioSystem;
        private readonly SoundFile[] _soundFileArray = new SoundFile[2];
        private EventFile _currentPlayingInstance;
        private EventFile _previousPlayingInstance;


        public SoundSystem()
        {

        }

        public  void Startup()
        {
            InitializeFmodStudio();
            var deserializedBanks = ReadMusicJsonFile("music");

            for (int i = 0; i < deserializedBanks.Bankfiles.Count; i++)
            {
                var bankFileModel = deserializedBanks.Bankfiles[i];
                var soundFile = new SoundFile(bankFileModel,_fmodStudioSystem);
                _soundFileArray[i] = soundFile;
            }

        }


        private void InitializeFmodStudio()
        {
            FMOD.Studio.System.create(out _fmodStudioSystem);
            _fmodStudioSystem.initialize(512, FMOD.Studio.INITFLAGS.NORMAL, FMOD.INITFLAGS.NORMAL, IntPtr.Zero);
        }

        BankfileList ReadMusicJsonFile(string fileName)
        {
            using var streamreader =
                Utils.Helpers.CreateJsonStreamReader(fileName, Utils.Helpers.ConfigFileToRead.MusicFile);
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

        public void PlayBgm(Enum enumValue)
        {
            if (_currentPlayingInstance != null)
                _previousPlayingInstance = _currentPlayingInstance;
            var value = Convert.ToInt32(enumValue);
            var chosenSoundFile = _soundFileArray[0].EventDescriptions[value];
            chosenSoundFile.Description.loadSampleData();
            chosenSoundFile.Description.createInstance(out var tempInstance);
            chosenSoundFile.Instance = tempInstance;
            chosenSoundFile.Instance.start();
            _currentPlayingInstance = chosenSoundFile;
            _previousPlayingInstance?.Instance.release();
        }
        
        public  void Update()
        {
            _fmodStudioSystem.update();
        }
    }

}
