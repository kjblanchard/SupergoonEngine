using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using FMOD;
using FMOD.Studio;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Utils;
using System = FMOD.System;

namespace SgEngine.Core
{
   public class ExtendedGame : Game
   {
       //public static Sound Sound { get; set; }
        protected GraphicsDeviceManager _graphics;
        protected SpriteBatch _spriteBatch;
        private FMOD.Studio.System fmodSystem;

        FMOD.Studio.Bank bank1;
        FMOD.Studio.Bank bank2;
        private FMOD.Studio.EventDescription description;


        private FMOD.Studio.EventInstance instance;





        public ExtendedGame()
        {
            _graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            //Sound = new Sound(Content);
            IsMouseVisible = true;    
        }

        protected override void Initialize()
        {
            base.Initialize();
            FMOD.Studio.System.create(out fmodSystem);

            fmodSystem.initialize(512, FMOD.Studio.INITFLAGS.NORMAL, FMOD.INITFLAGS.NORMAL, IntPtr.Zero);
            




            fmodSystem.loadBankFile("Content/Sound/Master.bank", FMOD.Studio.LOAD_BANK_FLAGS.NORMAL, out bank1);
            fmodSystem.loadBankFile("Content/Sound/Master.strings.bank", FMOD.Studio.LOAD_BANK_FLAGS.NORMAL, out bank2);


            fmodSystem.getEvent("event:/FirstEvent", out description);


            description.loadSampleData();


            description.createInstance(out instance);



            instance.start();
            FMOD.Studio.PLAYBACK_STATE state;
            instance.getPlaybackState(out state);

        }

        protected override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            fmodSystem.update();
        }
   }
}
