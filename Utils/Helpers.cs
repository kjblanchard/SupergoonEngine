using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace SgEngine.Utils
{
    public static class Helpers
    {
        public enum ConfigFileToRead
        {
            ConfigFile,
            MusicFile
        }
        public static StreamReader CreateJsonStreamReader(string fileToRead,ConfigFileToRead fileType)
        {
            string folderName = fileType switch
            {
                ConfigFileToRead.ConfigFile => "Base",
                ConfigFileToRead.MusicFile => "Sound",
                _ => throw new ArgumentOutOfRangeException(nameof(fileType), fileType, null)
            };

            return new StreamReader($"Config/{folderName}/{fileToRead}.json");
        }
    }
}
