using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Components;
using SgEngine.Core;
using SgEngine.Interfaces.EKS;
using SgEngine.Interfaces.SgDebug;
using SgEngine.SgDebug;

namespace SgEngine.EKS
{

    public class GameObject : IFullEksObject, ISendDebugMessage
    {
        #region State

        /// <summary>
        /// Event that the debug Handler will subscribe to for handling messages from gameobjects
        /// </summary>
        public event ISendDebugMessage.DebugLogEventHandler OnDebugMessage;
        /// <summary>
        /// The position of this game object, relative to its parent in the game-object hierachy.
        /// </summary>
        public Vector2 LocalPosition
        {
            get => _localPosition;
            set => _localPosition = value;

        }

        public static int id;

        public int Id => _id;
        protected int _id;
        protected Vector2 _localPosition = Vector2.Zero;

        public bool Debug
        {
            get => _debugMode;
            set
            {
                if (value)
                    OnDebugMessage += SgDebug.SgDebug.DebugMessage;
                else
                    OnDebugMessage -= SgDebug.SgDebug.DebugMessage;
                _debugMode = value;
            }
        }
        private bool _debugMode;

        /// <summary>
        /// Whether or not this game object is currently active.
        /// </summary>
        public bool IsActive { get; set; } = true;

        /// <summary>
        /// The (optional) parent of this object in the game-object hierarchy.
        /// If the object has a parent, then its position depends on its parent's position.
        /// </summary>
        public GameObject Parent { get; set; }

        public Vector2 GlobalPosition
        {
            get
            {
                if (Parent == null)
                    return LocalPosition;
                return LocalPosition + Parent.GlobalPosition;
            }
        }

        public List<Component> Components = new List<Component>();

        public List<Timer> Timers = new List<Timer>();
        #endregion


        #region Constructor

        /// <summary>
        /// Gameobject base class for everything (mostly)
        /// </summary>
        /// <param name="location">The location OR the offset of the gameobject, if it doesn't have a parent, its probably  </param>
        public GameObject()
        {
            _id = id;
            id++;
        }

        public GameObject(Vector2 location)
        {
            _localPosition = location;
        }

        public GameObject(GameObject parent, Vector2 offset)
        {
            _localPosition = offset;
            Parent = parent;
        }


        #endregion

        #region Functions

        public virtual void Initialize() { }

        public virtual void LoadContent() { }

        /// <summary>
        /// Updates this GameObject by one frame.
        /// By default, this method updates the object's position according to its velocity.
        /// You can override this method to create your own custom behavior.
        /// </summary>
        /// <param name="gameTime">An object containing information about the time that has passed.</param>

        public virtual void Update(GameTime gameTime)
        {
            for (int i = 0; i < Timers.Count; i++)
            {
                Timers[i].Update(gameTime);
            }

            for (int i = 0; i < Components.Count; i++)
            {
                Components[i].Update(gameTime);
            }
        }

        /// <summary>
        /// Draws this GameObject. By default, nothing happens, but other classes can override this method.
        /// </summary>
        /// <param name="gameTime">An object containing information about the time that has passed.</param>
        /// <param name="spriteBatch">The sprite batch to use.</param>
        public virtual void Draw(GameTime gameTime, SpriteBatch spriteBatch) { }

        /// <summary>
        /// Resets this game object to an initial state.
        /// For example, this can be useful for restarting a level of the game.
        /// Override this method so that it resets everything it needs to.
        /// </summary>
        public virtual void Reset() { }


        public void AddComponent(Component componentToAdd)
        {
            Components.Add(componentToAdd);
        }

        public Component GetComponent(Enum componentToGet)
        {
            return Components.FirstOrDefault(component => Equals(component._componentType, componentToGet));
        }

        public T GetComponent<T>(Enum componentToGet) where T : Component
        {
            var component = Components.FirstOrDefault(component => Equals(component._componentType, componentToGet));
            var convertedComponent = (T)(component);
            return convertedComponent;
        }

        public void AddTimer(Timer timerToAdd)
        {
            Timers.Add(timerToAdd);
        }


        public void SendDebugMessage(string messageToWrite, LogLevel logLevel = LogLevel.Debug)
        {
                OnDebugMessage?.Invoke(this, messageToWrite, logLevel);
        }
    }


    #endregion

}
