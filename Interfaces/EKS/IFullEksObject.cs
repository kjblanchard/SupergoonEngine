using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace SgEngine.Interfaces.EKS
{
    /// <summary>
    /// Brings in a few different interfaces, as well as a bunch of its own. Update, Draw, Handleinput, init, beginrun, activate and deactivate
    /// </summary>
    public interface IFullEksObject : IUpdate, IDraw, IHandleInput
    {
        public virtual void Initialize() { }
        public virtual void LoadContent() { }
        public virtual void BeginRun() { }
        public virtual void OnActivate() { }
        public virtual void OnDeactivate() { }
    }
}
