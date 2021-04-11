using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using FMOD;
using FMOD.Studio;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core.Sounds;
using SgEngine.Utils;
using System = FMOD.System;

namespace SgEngine.Core
{
   public class ExtendedGame : Game
   {
       public static SoundSystem SoundSystem { get; set; } = new SoundSystem();
        protected GraphicsDeviceManager _graphics;
        protected SpriteBatch _spriteBatch;
        //private FMOD.Studio.System fmodSystem;

        FMOD.Studio.Bank bank1;
        FMOD.Studio.Bank bank2;
        private FMOD.Studio.EventDescription description;


        private FMOD.Studio.EventInstance instance;





        public ExtendedGame()
        {
            _graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            IsMouseVisible = true;    
            SoundSystem.Startup();

        }

        protected override void Initialize()
        {
            base.Initialize();

        }

        protected override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            SoundSystem.Update();
        }
   }
}
