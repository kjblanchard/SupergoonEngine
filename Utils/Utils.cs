using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace SgEngine.Utils
{
    public static class Utils
    {
        public enum ConfigFileToRead
        {
            MusicFile
        }
        public static StreamReader CreateJsonStreamReader(string fileToRead,ConfigFileToRead fileType)
        {
            string folderName = fileType switch
            {
                ConfigFileToRead.MusicFile => "Sound",
                _ => throw new ArgumentOutOfRangeException(nameof(fileType), fileType, null)
            };

            return new StreamReader($"Config/{folderName}/{fileToRead}.json");
        }
    }
}
