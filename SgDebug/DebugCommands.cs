////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Linq;

namespace SgEngine.SgDebug
{
    public enum DebugCommands
    {
        Default,
        DebugMode,
        Exit,
    }

    public static class DebugDictionary
    {
        private static readonly Dictionary<string, DebugCommands> _debugLookupDictionary = new Dictionary<string, DebugCommands>
        {
            {"exit", DebugCommands.Exit},
            {"debug enable", DebugCommands.DebugMode},
        };

        public static DebugCommands GetDebugCommand(string lookupString)
        {
            _debugLookupDictionary.TryGetValue(lookupString, out var command);
            return command;
        }

        public static List<string> SearchForCommands(string lookupSubstring)
        {
            return (from stringDictKey in _debugLookupDictionary where stringDictKey.Key.StartsWith(lookupSubstring) select stringDictKey.Key).ToList();
        }
    }
}