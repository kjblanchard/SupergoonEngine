using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core;
using SgEngine.Core.Camera;
using SgEngine.Core.Input;
using SgEngine.Core.Sounds;
using SgEngine.Models;
using SgEngine.Utils;

namespace SgEngine.EKS
{
    public class GameWorld : Game
    {
        public static SoundSystem SoundSystem { get; set; } = new SoundSystem();
        public static InputSg Input { get; private set; } = new InputSg();
        public static UI.UI Ui { get; } = new UI.UI();

        public static GraphicsDevice GetGraphicsDevice()
        {
            return _instance.GraphicsDevice;
        }
        protected GraphicsDeviceManager _graphics;
        protected SpriteBatch _spriteBatch;
        protected ContentLoader _contentLoader;
        public static BaseConfig _baseConfig;

        public static Point ScreenSize => new Point(_baseConfig.Window.X, _baseConfig.Window.Y);
        public static Point WindowCenter => new Point(_baseConfig.World.X / 2, _baseConfig.World.Y / 2);
        public static GameWorld GetWorld => _instance;
        protected static GameWorld _instance;

        public ResolutionHelper ResolutionHelper;
        public Camera mainCamera;

        /// <summary>
        /// Gets the current playerController
        /// </summary>
        /// <param name="playerControllerToGet">What controller to get</param>
        /// <returns>The playercontroller at the spot that you want</returns>
        public static PlayerController GetPlayerController(int playerControllerToGet) =>
            Input.PlayerControllers[playerControllerToGet];

        public GameWorld()
        {
            _graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            IsMouseVisible = true;
            _instance ??= this;

        }

        protected override void Initialize()
        {
            base.Initialize();
            LoadBaseConfig();
            ConfigureGraphics();
            ResolutionHelper = new ResolutionHelper(_graphics, GraphicsDevice);
            mainCamera = new Camera(ResolutionHelper);
            ResolutionHelper.ApplyResolutionSettings(false);
            Input.Initialize();
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
            ResolutionHelper.windowSize = new Point(_baseConfig.Window.X, _baseConfig.Window.Y);
            ResolutionHelper.worldSize = new Point(_baseConfig.World.X, _baseConfig.World.Y);
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
            Input.Update();
            mainCamera.SetPlayerController(GetPlayerController(0));
            mainCamera.Update();
        }

        protected override void Draw(GameTime gameTime)
        {
            base.Draw(gameTime);

        }

    }
}
