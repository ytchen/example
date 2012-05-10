#include "Procedure.h"
#include "Event.h"

Event* schedule(double delta, void (*proc)(void))
{
   Procedure0 p0 = make_procedure(proc);
   return new Event(delta, p0);
}

