////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.IO;

namespace SgEngine.SgJson
{
    public class GameJsonLoader
    {
        
        public enum ConfigFileToRead
        {
            ConfigFile,
            MusicFile,
            UiScreenFile
        }
        public static StreamReader CreateJsonStreamReader(string fileToRead,ConfigFileToRead fileType)
        {
            string folderName = fileType switch
            {
                ConfigFileToRead.ConfigFile => "Base",
                ConfigFileToRead.MusicFile => "Sound",
                ConfigFileToRead.UiScreenFile => "Ui/Screens",
                _ => throw new ArgumentOutOfRangeException(nameof(fileType), fileType, null)
            };

            return new StreamReader($"Config/{folderName}/{fileToRead}.json");
        }
    }
}