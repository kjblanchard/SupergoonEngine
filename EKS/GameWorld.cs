using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Core;
using SgEngine.Core.Camera;
using SgEngine.Core.Input;
using SgEngine.Core.Sounds;
using SgEngine.GUI;
using SgEngine.Models;
using SgEngine.Utils;

namespace SgEngine.EKS
{
    public class GameWorld : Game
    {
        public static SoundSystem SoundSystem => _instance._soundSystem;
        public static InputSg Input => _instance._input;
        public static Gui Gui => _instance._gui;

        public static GraphicsDevice GetGraphicsDevice => _instance.GraphicsDevice;
        public static Point WindowSize => new Point(_instance._baseConfig.Window.X, _instance._baseConfig.Window.Y);
        public static Point WindowCenter => new Point(_instance._baseConfig.World.X / 2, _instance._baseConfig.World.Y / 2);
        public static GameWorld GetWorld => _instance;
        protected static GameWorld _instance;
        public static Camera MainCamera => _instance._mainCamera;
        public static Matrix ResolutionSpriteScale => _instance._resolutionHelper.SpriteScale;

        /// <summary>
        /// Gets whether the game is currently full screen.
        /// When you set it, it also will Apply the resolution changes afterwards
        /// </summary>
        public static bool FullScreen
        {
            get => _instance._fullScreen;
            set
            {
                _instance._fullScreen = value;
                _instance._resolutionHelper.ApplyResolutionSettings(_instance._fullScreen);
            }
        }

        protected bool _fullScreen;
        protected GraphicsDeviceManager _graphicsDeviceManager;
        protected SpriteBatch _spriteBatch;
        protected ContentLoader _contentLoader;
        protected ResolutionHelper _resolutionHelper;
        protected Camera _mainCamera;
        protected Gui _gui;
        protected SoundSystem _soundSystem;
        protected InputSg _input;
        protected BaseConfig _baseConfig;

        /// <summary>
        /// Gets the current playerController
        /// </summary>
        /// <param name="playerControllerToGet">What controller to get</param>
        /// <returns>The playercontroller at the spot that you want</returns>
        public static PlayerController GetPlayerController(int playerControllerToGet) =>
            Input.PlayerControllers[playerControllerToGet];
        /// <summary>
        /// The full game world, this holds all the important things like graphics, UI, soundsystem, input, etc
        /// </summary>
        public GameWorld()
        {
            _graphicsDeviceManager = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            IsMouseVisible = true;
            _instance ??= this;
            _gui = new Gui();
            _soundSystem = new SoundSystem();
            _input = new InputSg();
        }

        protected override void Initialize()
        {
            base.Initialize();
            Input.Initialize();
            SoundSystem.Initialize();
            _baseConfig = LoadBaseConfig();
            InitializeResolutionHelper();
            _mainCamera = new Camera(_resolutionHelper, GraphicsDevice);
            _spriteBatch = new SpriteBatch(GraphicsDevice);
        }

        /// <summary>
        /// Creates the resolutionHelper and initializes it
        /// </summary>
        private void InitializeResolutionHelper()
        {
            _resolutionHelper = new ResolutionHelper(_graphicsDeviceManager, GraphicsDevice);
            _resolutionHelper.Initialize(new Point(_baseConfig.Window.X, _baseConfig.Window.Y),
                new Point(_baseConfig.World.X, _baseConfig.World.Y));
            _resolutionHelper.ApplyResolutionSettings(false);
        }

        /// <summary>
        /// Load the base configuration file from json, and store it as a local variable;
        /// </summary>
        private BaseConfig LoadBaseConfig()
        {
            using var streamreader =
                Helpers.CreateJsonStreamReader("BaseConfig", Helpers.ConfigFileToRead.ConfigFile);
            var data = streamreader.ReadToEnd();
            return System.Text.Json.JsonSerializer.Deserialize<BaseConfig>(data);
        }

        protected override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            SoundSystem.Update();
            Input.Update();
            MainCamera.Update();
        }
    }
}
