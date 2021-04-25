////////////////////////////////////////////////////////////
//
// Super Goon Games - 2D Game Engine
// Copyright (C) 2020-2021 - Kevin Blanchard
//
////////////////////////////////////////////////////////////

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using MonoGame.Extended;
using MonoGame.Extended.Tweening;
using SgEngine.EKS;

namespace SgEngine.Components
{
    public class TweeningComponent : Component
    {
        public List<Tweener> CurrentTweens = new List<Tweener>();
        public List<Tweener> EndedTweens = new List<Tweener>();
        public List<Tweener> OldTweens = new List<Tweener>();

        public TweeningComponent(GameObject parent) : base(parent)
        {
            _componentType = EngineComponentTypes.TweeningComponent;
        }

        public void TweenEnd(Tweener tweenThatEnded)
        {
            EndedTweens.Add(tweenThatEnded);
        }

        private void CompleteTweens()
        {
            OldTweens.AddRange(EndedTweens);
            EndedTweens.Clear();
        }

        public void AddTween(Tweener tweenToAdd)
        {
            CurrentTweens.Add(tweenToAdd);
        }

        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);
            foreach (var tween in CurrentTweens)
            {
                tween.Update(gameTime.GetElapsedSeconds());
            }
            if (EndedTweens.Count > 0)
                CompleteTweens();
        }
    }
}