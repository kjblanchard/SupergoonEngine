////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using SgEngine.SgDebug;

namespace SgEngine.Interfaces.SgDebug
{
    public interface ISendDebugMessage
    {
        public delegate void DebugLogEventHandler(object item, string data, LogLevel logLevel);

        public event DebugLogEventHandler OnDebugMessage;
        public void SendDebugMessage(string messageToWrite, LogLevel logLevel = LogLevel.Debug);

    }

}