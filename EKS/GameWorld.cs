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


        public static Point windowSize;
        public static Point worldSize;
        public static Matrix spriteScale;

        public static Camera mainCamera = new Camera();

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
            //ResolutionHelper.Init(_graphics, GraphicsDevice);
            ApplyResolutionSettings(false);
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
            //ResolutionHelper.windowSize = new Point(_baseConfig.Window.X, _baseConfig.Window.Y);
            //ResolutionHelper.worldSize = new Point(_baseConfig.World.X, _baseConfig.World.Y);
            
            windowSize = new Point(_baseConfig.Window.X, _baseConfig.Window.Y);
            worldSize = new Point(_baseConfig.World.X, _baseConfig.World.Y);
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
            if (GetPlayerController(0).IsButtonPressed(ControllerButtons.B))
            {
                ResolutionHelper.ToggleFullScreen();
            }
            mainCamera.SetPlayerController(GetPlayerController(0));
            mainCamera.Update();
        }

        protected override void Draw(GameTime gameTime)
        {
            base.Draw(gameTime);

        }
        void ApplyResolutionSettings(bool fullScreen)
        {
            // make the game full-screen or not
            _graphics.IsFullScreen = fullScreen;

            // get the size of the screen to use: either the window size or the full screen size
            Point screenSize;
            if (fullScreen)
                screenSize = new Point(GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Width, GraphicsAdapter.DefaultAdapter.CurrentDisplayMode.Height);
            else
                screenSize = windowSize;

            // scale the window to the desired size
            _graphics.PreferredBackBufferWidth = screenSize.X;
            _graphics.PreferredBackBufferHeight = screenSize.Y;

            _graphics.ApplyChanges();

            // calculate and set the viewport to use
            GraphicsDevice.Viewport = CalculateViewport(screenSize);

            // calculate how the graphics should be scaled, so that the game world fits inside the window
            spriteScale = Matrix.CreateScale((float)GraphicsDevice.Viewport.Width / worldSize.X, (float)GraphicsDevice.Viewport.Height / worldSize.Y, 1);
            //spriteScale.Translation = (new Vector3(100,0,0));
        }

        /// <summary>
        /// Calculates and returns the viewport to use, so that the game world fits on the screen while preserving its aspect ratio.
        /// </summary>
        /// <param name="windowSize">The size of the screen on which the world should be drawn.</param>
        /// <returns>A Viewport object that will show the game world as large as possible while preserving its aspect ratio.</returns>
        Viewport CalculateViewport(Point windowSize)
        {
            // create a Viewport object
            Viewport viewport = new Viewport();

            // calculate the two aspect ratios
            float gameAspectRatio = (float)worldSize.X / worldSize.Y;
            float windowAspectRatio = (float)windowSize.X / windowSize.Y;

            // if the window is relatively wide, use the full window height
            if (windowAspectRatio > gameAspectRatio)
            {
                viewport.Width = (int)(windowSize.Y * gameAspectRatio);
                viewport.Height = windowSize.Y;
            }
            // if the window is relatively high, use the full window width
            else
            {
                viewport.Width = windowSize.X;
                viewport.Height = (int)(windowSize.X / gameAspectRatio);
            }

            // calculate and store the top-left corner of the viewport
            viewport.X = (windowSize.X - viewport.Width) / 2;
            viewport.Y = (windowSize.Y - viewport.Height) / 2;

            return viewport;
        }

        public Vector2 ScreenToWorld(Vector2 screenPosition)
        {
            var viewportTopLeft = new Vector2(GraphicsDevice.Viewport.X, GraphicsDevice.Viewport.Y);
            var screenToWorldScale = worldSize.X / (float) GraphicsDevice.Viewport.Width;
            return (screenPosition - viewportTopLeft) * screenToWorldScale;
        }

    }
}
