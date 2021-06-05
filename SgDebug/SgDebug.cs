////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.IO;
using Microsoft.Xna.Framework;
using SgEngine.EKS;
using SgEngine.GUI.Components;

namespace SgEngine.SgDebug
{
    public enum LogLevel
    {
        Debug = 0,
        LogFile = 1,
        Critical = 2
    }
    public static class SgDebug
    {
        private const string _logFolder = "Logs";
        private const string _filePath = _logFolder + "debugLog.txt";
        private const string _errorPath = _logFolder + "errorLog.txt";

        public static void Initialize()
        {
            CreateLogDirectory();
        }

        private static void CreateLogDirectory()
        {
            if (Directory.Exists(_logFolder)) return;
            WriteDebugLog("Creating log directory");
            Directory.CreateDirectory(_logFolder);
        }

        public static void DebugMessage(object sender, string whatToWrite, LogLevel logLevel)
        {

            switch (logLevel)
            {
                case LogLevel.Debug:
                    WriteDebugLog(sender, whatToWrite);
                    break;
                case LogLevel.LogFile:
                    WriteLogFileLog(sender, whatToWrite);
                    break;
                case LogLevel.Critical:
                    WriteCriticalLog(sender, whatToWrite);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(logLevel), logLevel, null);
            }
        }

        private static void WriteDebugLog(string whatToWrite)
        {
            var newString = $"{DateTime.Now}-DbgMsg-{whatToWrite}";
            Debug.WriteLine(newString);
            SgDebugPanel.AddToDisplayHistory(newString,Color.Blue);
        }

        private static void WriteDebugLog(object sender, string whatToWrite)
        {
            WriteDebugLog(whatToWrite);
            switch (sender)
            {
                case GameObject gameObject:
                    WriteObjectDebugLog(gameObject);
                    break;
                case GuiComponent guiComponent:
                    WriteObjectDebugLog(guiComponent);
                    break;
            }

        }

        private static void WriteObjectDebugLog(GameObject gameObject)
        {
            Debug.WriteLine($"From: GameObject: {gameObject?.Id} ");
        }
        private static void WriteObjectDebugLog(GuiComponent guiComponent)
        {
            Debug.WriteLine($"From: GuiComponent: {guiComponent?.UiObjectNumber} ");
        }
        private static void WriteLogFileLog(object gameObject, string whatToWrite)
        {
            using var streamReader = new StreamWriter(_filePath, true);
            streamReader.WriteLineAsync(whatToWrite);
        }
        private static void WriteCriticalLog(object gameObject, string whatToWrite)
        {
            using var streamReader = new StreamWriter(_errorPath, true);
            streamReader.WriteLineAsync(whatToWrite);
        }
    }
}