using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Components;
using SgEngine.Core;
using SgEngine.Interfaces;

namespace SgEngine.EKS
{

    public class GameObject : IUpdate
    {
        #region State

        /// <summary>
        /// The position of this game object, relative to its parent in the game-object hierachy.
        /// </summary>
        public Vector2 LocalPosition
        {
            get => _localPosition;
            set => _localPosition = value;

        }
        protected Vector2 _localPosition = Vector2.Zero;


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

        public void AddTimer(Timer timerToAdd)
        {
            Timers.Add(timerToAdd);
        }
    }


    #endregion

}
