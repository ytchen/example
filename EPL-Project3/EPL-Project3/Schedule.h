#if !defined(__SCHEDULE_H__)
#define __SCHEDULE_H__

#include "Procedure.h"
#include "Event.h"
#include "Params.h"

#if 0
Event* schedule(double delta, void (*proc)(void));
template <class Arg1>
   Event* schedule(double delta, void (*proc)(Arg1), Arg1 arg1);
template <class Arg1, class Arg2>
   Event* schedule(double delta, void (*proc)(Arg1, Arg2), Arg1 arg1, Arg2 arg2);

template <class Obj>
   Event* schedule(double delta, Obj& obj, void (Obj::*proc)(void));
template <class Obj, class Arg1>
   Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1), Arg1 arg1);
template <class Obj, class Arg1, class Arg2>
   Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1,Arg2), Arg1 arg1, Arg2 arg2);

/* template <class Obj, class Arg1>
   Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1,bool*), Arg1 arg1, bool* arg2);
template <class Obj, class Arg1>
   Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1,String,bool*),
                                                Arg1 arg1, String arg2, bool* arg3); */
#endif


/*-------------------------------------------------------------*/
// Scheduling Events

#if !defined(Delete)
#define Delete(x) (delete (x), (x) = NULL)
#endif

#if !defined(unschedule)
#define unschedule(event) { if (event != NULL) { Delete(event); } }
#endif


Event* schedule(double delta, void (*proc)(void));

template <class Arg1>
Event* schedule(double delta, void (*proc)(Arg1), Arg1 arg1)
{
   Procedure0 p0 = make_procedure<Arg1>(proc).bind(arg1);
   return new Event(delta, p0);
}

template <class Arg1, class Arg2>
Event* schedule(double delta, void (*proc)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
{
   Procedure0 p0 = make_procedure<Arg1,Arg2>(proc).bind(arg1).bind(arg2);
   return new Event(delta, p0);
}




template <class Obj>
Event* schedule(double delta, Obj& obj, void (Obj::*proc)(void))
{
   Procedure0 p0 = make_procedure<Obj>(obj, proc);
   return new Event(delta, p0);
}

template <class Obj, class Arg1>
Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1), Arg1 arg1)
{
   Procedure0 p0 = make_procedure<Obj,Arg1>(obj, proc).bind(arg1);
   return new Event(delta, p0);
}


template <class Obj, class Arg1, class Arg2>
Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1, Arg2),
                              Arg1 arg1, Arg2 arg2)
{
   Procedure0 p0 = make_procedure<Obj,Arg1,Arg2>(obj, proc).bind(arg1).bind(arg2);
   return new Event(delta, p0);
}


#if 0
template <class Obj, class Arg1>
Event* schedule(double delta, Obj& obj, void (Obj::*proc)(Arg1, bool*),
                              Arg1 arg1, bool* arg2)
{
   Procedure0 p0 = make_procedure<Obj,Arg1>(obj, proc).bind(arg1).bind(arg2);
   return new Event(delta, p0);
}
#endif

#endif // !defined(__SCHEDULE_H__)
