#include <Supergoon/services.h>
extern void InitializeServiceSystemImpl(void);
extern void UpdateServiceSystemImpl(void);
extern void ShutdownServiceSystemImpl(void);

void InitializeServiceSystem(void) { InitializeServiceSystemImpl(); }
void UpdateServiceSystem(void) { UpdateServiceSystemImpl(); }
void ShutdownServiceSystem(void) { ShutdownServiceSystemImpl(); }
