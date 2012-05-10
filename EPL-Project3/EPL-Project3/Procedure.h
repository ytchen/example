#if !(defined(_Procedure_h))
#define _Procedure_h 1

#include "Wrapper.h"

class _Procedure0 {
public:
  virtual void operator()(void) {}
  virtual _Procedure0* copy(void) { return new _Procedure0(*this); }
  virtual bool operates_on(void*) { return false; }
  virtual ~_Procedure0(void) {}
};

class _Procedure0_fun : public _Procedure0 {
  void (*fun)(void);
public:
  _Procedure0_fun(void (*_fun)(void)) : fun(_fun) {}
  void operator()(void) { (*fun)(); }
  _Procedure0* copy(void) { return new _Procedure0_fun(*this); }
  bool operates_on(void*) { return false; }
  _Procedure0_fun(void) {}
};


class Procedure0 : public Wrapper<_Procedure0> {
public:
  Procedure0(void) : Wrapper<_Procedure0 >(new _Procedure0) {}
  Procedure0(_Procedure0* p) : Wrapper<_Procedure0 >(p) {}
  void operator()(void) { (*core)(); }
  bool operates_on(void* p) { return core->operates_on(p); }
  ~Procedure0(void) {}
};

inline
Procedure0 make_procedure(void (*p)(void))
{
  return Procedure0(new _Procedure0_fun(p));
}


template <class Obj>
class _Procedure0_objbound : public _Procedure0 {
  typedef void (Obj::*Fun)(void);
  Obj& obj;
  Fun fun;
public:
  _Procedure0_objbound(Obj& x, Fun f) : obj(x), fun(f) {}
  ~_Procedure0_objbound(void) {}
  void operator()(void) { (obj.*fun)(); }
  _Procedure0* copy(void) { 
    return new _Procedure0_objbound<Obj>(*this); 
  }
  bool operates_on(void* p) { return &obj == p; }
};

template <class Obj>
Procedure0 make_procedure(Obj& x, void (Obj::*fun)(void))
{
  return Procedure0(new _Procedure0_objbound<Obj>(x,fun));
}


template <class Arg1>
class _Procedure1 {
  typedef _Procedure1<Arg1> Same;
  typedef _Procedure1<Arg1> Base;
public:
  virtual void operator()(Arg1) { }
  virtual ~_Procedure1(void) {}
  virtual Base* copy(void) { return new Same(*this);  }
  virtual bool operates_on(void*) { return false; }
};

template <class Arg1>
class _Procedure1_fun  : public _Procedure1<Arg1> {
  typedef _Procedure1_fun<Arg1> Same;
  typedef _Procedure1<Arg1> Base;
  void (*fun)(Arg1);
public:
  ~_Procedure1_fun(void) {}
  void operator()(Arg1 arg1) { 
    (*fun)(arg1); 
  }
  Base* copy(void) { return new Same(*this);  }
  _Procedure1_fun(void (*f)(Arg1)) : fun(f) {}
  bool operates_on(void*) { return false; }
};


template <class Arg1>
class Procedure1 : public Wrapper< _Procedure1<Arg1> > {
  typedef _Procedure1<Arg1> Core;
  typedef Wrapper< Core > Base;
public:
  Procedure1(void) : Base((Core*) new _Procedure1<Arg1>) {}
  Procedure1(Core*p) : Base(p) {}
  ~Procedure1(void) {}
  void operator()(Arg1 x) { (*Wrapper<Core>::core)(x); }
  bool operates_on(void* p) { return Wrapper<Core>::core->operates_on(p); }
  Procedure0 bind(Arg1 arg1);
};

template <class Arg1>
Procedure1<Arg1> make_procedure( void (*fun)(Arg1)) 
{
  return Procedure1<Arg1>(new _Procedure1_fun<Arg1>(fun));
}

template <class Arg1>
Procedure1<Arg1&> make_procedure(void (*fun)(Arg1&))
{
  return Procedure1<Arg1&>(new _Procedure1_fun<Arg1&>(fun));
}

template <class Obj, class Arg1>
class _Procedure1_objbound : public _Procedure1<Arg1> {
  typedef _Procedure1_objbound<Obj,Arg1> Same;
  typedef _Procedure1<Arg1> Base;
  typedef void (Obj::*Fun)(Arg1);
  Obj& obj;
  Fun fun;
public:
  _Procedure1_objbound(Obj& x, Fun f) : obj(x), fun(f) {}
  ~_Procedure1_objbound(void) {}
  void operator()(Arg1 arg1) { (obj.*fun)(arg1); }
  Base* copy(void) {
    return new Same(*this);
  }
  bool operates_on(void* p) { return &obj == p; }
};

template <class Obj, class Arg1>
Procedure1<Arg1> make_procedure(Obj& x, void (Obj::*fun)(Arg1))
{
  return Procedure1<Arg1>(new _Procedure1_objbound<Obj,Arg1>(x,fun));
}

template <class Arg1>
class _Procedure1_bound : public _Procedure0 {
  typedef _Procedure1_bound<Arg1> Same;
  typedef _Procedure0 Base;
  Arg1 arg1;
  Procedure1<Arg1> fun;
public:
  _Procedure1_bound(Arg1 a1, Procedure1<Arg1> f)
    : arg1(a1), fun(f) {}
  ~_Procedure1_bound(void) {}
  Base* copy(void) { return new Same(*this); }
  void operator()(void) { fun(arg1); }
  bool operates_on(void* p) { return fun.operates_on(p); }
};

template <class Arg1>
Procedure0
Procedure1<Arg1>::bind(Arg1 arg1)
{
  return Procedure0(new _Procedure1_bound<Arg1>(arg1, *this));
}


template <class Arg1, class Arg2>
class _Procedure2 {
  typedef _Procedure2<Arg1,Arg2> Same;
  typedef _Procedure2<Arg1,Arg2> Base;
public:
  virtual ~_Procedure2(void) {}
  virtual void operator()(Arg1,Arg2) {}
  virtual Base* copy(void) { return new Same; }
  virtual bool operates_on(void*) { return false; }
};


template <class Arg1, class Arg2>
class _Procedure2_fun : public _Procedure2<Arg1,Arg2>{
  typedef _Procedure2_fun<Arg1,Arg2> Same;
  typedef _Procedure2<Arg1,Arg2> Base;
  void (*fun)(Arg1, Arg2);
public:
  ~_Procedure2_fun(void) {}
  void operator()(Arg1 arg1, Arg2 arg2) { 
    (*fun)(arg1, arg2); 
  }
  Base* copy(void) { 
    return new Same(*this); 
  }
  _Procedure2_fun(void (*f)(Arg1,Arg2)) : fun(f) {}
  bool operates_on(void*) { return false; }
};

template <class Arg1, class Arg2>
class Procedure2 : public Wrapper<_Procedure2<Arg1,Arg2> > {
  typedef _Procedure2<Arg1,Arg2> Core;
  typedef Wrapper<Core> Base;
public:
  Procedure2(void) : Base(new _Procedure2<Arg1,Arg2>) {}
  Procedure2(Core* p) : Base(p) {}
  ~Procedure2(void) {}
  void operator()(Arg1 arg1, Arg2 arg2) { (*Wrapper<Core>::core)(arg1,arg2); }
  Procedure1<Arg2> bind1st(Arg1);
  Procedure1<Arg2> bind(Arg1 arg1) { return bind1st(arg1); }
  Procedure1<Arg1> bind2nd(Arg2);
  bool operates_on(void* p) { return Wrapper<Core>::core->operates_on(p); }
};

template <class Arg1, class Arg2>
Procedure2<Arg1, Arg2> make_procedure( void (*fun)(Arg1,Arg2)) 
{
  return Procedure2<Arg1,Arg2>(new _Procedure2_fun<Arg1,Arg2>(fun));
}

template <class Arg1, class Arg2>
class _Procedure2_bound1st : public _Procedure1<Arg2> {
  typedef _Procedure2_bound1st<Arg1,Arg2> Same;
  typedef _Procedure1<Arg2> Base;
  const Arg1 arg1;
  Procedure2<Arg1,Arg2> fun;
public:
  _Procedure2_bound1st(Arg1 a1, Procedure2<Arg1,Arg2> f)
    : arg1(a1), fun(f) {}
  ~_Procedure2_bound1st(void) {}
  Base* copy(void) {
    return new Same(*this);
  }
  void operator()(Arg2 arg2) {
    fun(arg1,arg2);
  }
  bool operates_on(void* p) { return fun.operates_on(p); }
};



template <class Obj, class Arg1, class Arg2>
class _Procedure2_objbound : public _Procedure2<Arg1,Arg2> {
  typedef _Procedure2_objbound<Obj,Arg1,Arg2> Same;
  typedef _Procedure2<Arg1,Arg2> Base;
  typedef void (Obj::*Fun)(Arg1, Arg2);
  Obj& obj;
  Fun fun;
public:
  _Procedure2_objbound(Obj& x, Fun f) : obj(x), fun(f) {}
  ~_Procedure2_objbound(void) {}
  void operator()(Arg1 arg1, Arg2 arg2) { (obj.*fun)(arg1,arg2); }
  Base* copy(void) {
    return new Same(*this);
  }
  bool operates_on(void* p) { return &obj == p;}
};


template <class Obj, class Arg1, class Arg2>
Procedure2<Arg1,Arg2> make_procedure(Obj& x, void (Obj::*fun)(Arg1,Arg2))
{
  return 
    Procedure2<Arg1,Arg2>(new 
                          _Procedure2_objbound<Obj,Arg1,Arg2>(x,fun));
}

template <class Arg1, class Arg2>
class _Procedure2_bound2nd : public _Procedure1<Arg1> {
  typedef _Procedure2_bound2nd<Arg1,Arg2> Same;
  typedef _Procedure1<Arg1> Base;
  const Arg2 arg2;
  Procedure2<Arg1,Arg2> fun;
public:
  _Procedure2_bound2nd(Arg2 a2, Procedure2<Arg1,Arg2> f)
    : arg2(a2), fun(f) {}
  ~_Procedure2_bound2nd(void){}
  Base* copy(void) {
    return new Same(*this);
  }
  void operator()(Arg1 arg1) {
    fun(arg1,arg2);
  }
  bool operates_on(void* p) { return fun.operates_on(p); }
};

template <class Arg1, class Arg2>
Procedure1<Arg2>
Procedure2<Arg1,Arg2>::bind1st(Arg1 arg1)
{
  return 
    Procedure1<Arg2>(new 
                     _Procedure2_bound1st<Arg1,Arg2> (arg1, *this));
}


template <class Arg1, class Arg2>
Procedure1<Arg1>
Procedure2<Arg1,Arg2>::bind2nd(Arg2 arg2)
{
  return 
    Procedure1<Arg1>(new 
                     _Procedure2_bound2nd<Arg1,Arg2> (arg2, *this));
}

template <class Arg1, class Arg2, class Arg3>
class _Procedure3 {
  typedef _Procedure3<Arg1,Arg2,Arg3> Same;
  typedef _Procedure3<Arg1,Arg2,Arg3> Base;
public:
  virtual ~_Procedure3(void) {}
  virtual void operator()(Arg1, Arg2, Arg3) { }
  virtual Base* copy(void) {
    return new Same(*this);
  }
  virtual bool operates_on(void*) { return false; }
};

template <class Arg1, class Arg2, class Arg3>
class _Procedure3_fun : public _Procedure3<Arg1,Arg2,Arg3> {
  typedef _Procedure3<Arg1,Arg2,Arg3> Base;
  typedef _Procedure3_fun<Arg1,Arg2,Arg3> Same;
  void (*fun)(Arg1, Arg2, Arg3);
public:
  ~_Procedure3_fun(void) {}
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    (*fun)(arg1, arg2, arg3);
  }
  Base* copy(void) {
    return new Same(*this);
  }
  _Procedure3_fun(void (*f)(Arg1,Arg2,Arg3)) : fun(f) {}
  bool operates_on(void*) { return false; }
};


template <class Arg1, class Arg2, class Arg3>
class Procedure3 : public Wrapper<_Procedure3<Arg1,Arg2,Arg3> > {
  typedef _Procedure3<Arg1,Arg2,Arg3> Core;
  typedef Wrapper<Core> Base;
public:
  Procedure3(void) : Base(new _Procedure3<Arg1,Arg2,Arg3>) {}
  Procedure3(Core* p) : Base(p) {}
  ~Procedure3(void) {}
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    (*Wrapper<Core>::core)(arg1,arg2,arg3);
  }
  Procedure2<Arg2,Arg3> bind(Arg1);
  bool operates_on(void* p) { return Wrapper<Core>::core->operates_on(p); }
};

template <class Arg1, class Arg2, class Arg3>
Procedure3<Arg1,Arg2,Arg3> 
make_procedure(void (*fun)(Arg1,Arg2,Arg3))
{
  return 
    Procedure3<Arg1,Arg2,Arg3>(new 
                               _Procedure3_fun<Arg1,Arg2,Arg3>(fun));
}


template <class Obj, class Arg1, class Arg2, class Arg3>
class _Procedure3_objbound : public _Procedure3<Arg1,Arg2,Arg3> {
  typedef _Procedure3_objbound<Obj,Arg1,Arg2,Arg3> Same;
  typedef _Procedure3<Arg1,Arg2,Arg3> Base;
  typedef void (Obj::*Fun)(Arg1,Arg2,Arg3);
  Obj& obj;
  Fun fun;
public:
  _Procedure3_objbound(Obj& x, Fun f) : obj(x), fun(f) {}
  ~_Procedure3_objbound(void) {}
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    (obj.*fun)(arg1,arg2,arg3);
  }
  Base* copy(void) {
    return new Same(*this);
  }
  bool operates_on(void* p) { return &obj == p; }
};

template <class Obj, class Arg1, class Arg2, class Arg3>
Procedure3<Arg1,Arg2,Arg3> 
make_procedure(Obj& x, void (Obj::*fun)(Arg1,Arg2,Arg3))
{
  typedef _Procedure3_objbound<Obj,Arg1,Arg2,Arg3> Core;
  typedef Procedure3<Arg1,Arg2,Arg3> Wrap;
  return Wrap(new Core(x, fun));
}

template <class Arg1, class Arg2, class Arg3>
class _Procedure3_bound : public _Procedure2<Arg2,Arg3>
{
  typedef _Procedure3_bound<Arg1,Arg2,Arg3> Same;
  typedef _Procedure2<Arg2,Arg3> Base;
  typedef Procedure3<Arg1,Arg2,Arg3> Parent;
  const Arg1 arg1;
  Parent fun;
public:
  _Procedure3_bound(Arg1 a1, Parent f) : arg1(a1), fun(f) {}
  ~_Procedure3_bound(void) {}
  Base* copy(void) {
    return new Same(*this);
  }

  void operator()(Arg2 arg2, Arg3 arg3) {
    fun(arg1,arg2,arg3);
  }

  bool operates_on(void* p) { return fun.operates_on(p); }
};

template <class Arg1, class Arg2, class Arg3>
Procedure2<Arg2,Arg3>
Procedure3<Arg1,Arg2,Arg3>::bind(Arg1 arg1)
{
  return
    Procedure2<Arg2,Arg3>(new
                          _Procedure3_bound<Arg1,Arg2,Arg3>(arg1,*this));

}

#endif /* !(defined(_Procedure_h)) */
