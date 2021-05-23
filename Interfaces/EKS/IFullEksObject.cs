using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Interfaces.EKS
{
    public interface IFullEksObject : IUpdate, IDraw, IHandleInput
    {
        public virtual void Initialize() { }
        public virtual void LoadContent() { }
        public virtual void BeginRun() { }
        public virtual void OnActivate() { }
        public virtual void OnDeactivate() { }
    }
}
