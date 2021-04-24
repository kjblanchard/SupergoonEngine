using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Mime;
using System.Text;
using FMOD;
using FMOD.Studio;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core;
using SgEngine.Core.Sounds;
using SgEngine.Models;
using SgEngine.Utils;

namespace SgEngine.EKS
{
   public class GameWorld : Game
   {
       public static SoundSystem SoundSystem { get; set; } = new SoundSystem();
        protected GraphicsDeviceManager _graphics;
        protected SpriteBatch _spriteBatch;
        protected ContentLoader _contentLoader;
        public static BaseConfig _baseConfig;

        public static GameWorld GetWorld => _instance;
        protected static GameWorld _instance;

        public GameWorld()
        {
            _graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";

            IsMouseVisible = true;
            _instance ??= this;
            //_contentLoader = new ContentLoader(Content);

        }

        protected override void Initialize()
        {
            base.Initialize();
            LoadBaseConfig();
            ConfigureGraphics();
            SoundSystem.Startup();

        }

        private void LoadBaseConfig()
        {
            using var streamreader =
                Helpers.CreateJsonStreamReader("BaseConfig", Helpers.ConfigFileToRead.ConfigFile);
            var data = streamreader.ReadToEnd();
            _baseConfig = System.Text.Json.JsonSerializer.Deserialize<BaseConfig>(data);
        }

        private void ConfigureGraphics()
        {
            _graphics.PreferredBackBufferWidth = _baseConfig.Window.X;
            _graphics.PreferredBackBufferHeight = _baseConfig.Window.Y;
            _graphics.ApplyChanges();
        }

        protected override void LoadContent()
        {
            base.LoadContent();
            _spriteBatch = new SpriteBatch(GraphicsDevice);
        }

        protected override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            SoundSystem.Update();
        }

        protected override void Draw(GameTime gameTime)
        {
            base.Draw(gameTime);

        }
   }
}
