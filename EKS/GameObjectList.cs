using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using SgEngine.Interfaces;
using SgEngine.Interfaces.EKS;

namespace SgEngine.EKS
{
    public class GameObjectList<T> : IFullEksObject where T : GameObject
    {

        public List<T> NewGameObjects { get; set; } = new List<T>();
        public List<T> GameObjects { get; set; } = new List<T>();

        public void Initialize()
        {
            StartupGameObjects();
            MoveNewGameObjToFullGameObjList();
        }

        public void Update(GameTime gameTime)
        {
            StartupGameObjects();
            MoveNewGameObjToFullGameObjList();
            UpdateContentGameObjects(gameTime);
        }

        private void StartupGameObjects()
        {
            foreach (var newGameObject in NewGameObjects)
            {
                newGameObject.Initialize();
                newGameObject.LoadContent();
            }
        }

        private void MoveNewGameObjToFullGameObjList()
        {
            GameObjects.AddRange(NewGameObjects);
            NewGameObjects.Clear();
        }
        private void UpdateContentGameObjects(GameTime gameTime)
        {
            foreach (var gameObject in GameObjects)
            {
                gameObject.Update(gameTime);
            }
        }
        public void Draw(GameTime gameTime, SpriteBatch spriteBatch)
        {
            foreach (var gameObject in GameObjects)
            {
                gameObject.Draw(gameTime,spriteBatch);
            }
        }

        public void AddGameObject(T gameObject)
        {
            NewGameObjects.Add(gameObject);
        }


    }
}
