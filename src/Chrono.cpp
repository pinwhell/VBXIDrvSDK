#include <Kernel/Decls.h>
#include <Kernel/Chrono.h>

REGPARAMDECL(unsigned long) msleep(unsigned int msecs);
REGPARAMDECL(unsigned long) msleep_interruptible(unsigned int msecs);

void SleepMillis(unsigned int millis, bool bInterrumpible)
{
	bInterrumpible = &msleep_interruptible ? bInterrumpible : false;
	(bInterrumpible ? msleep_interruptible : msleep)(millis);
}