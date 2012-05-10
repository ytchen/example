#if !(_Wrapper_h)
#define _Wrapper_h 1
#include <typeinfo>

/*
 * A generic wrapper.
 * place this around non-abstract base classes and you'll inherit 
 * copy-on-write with a virtual copy constructor.
 *
 * For example:
 *
 *   class _Shape {                
 *     virtual void copy(void) const; <-- REQUIRED
 *        < you can add your own virtual functions >
 *     virtual void draw(void) const; <-- for example
 *     virtual void rotate(int); 
 *   };
 *
 *   class _Rectangle : public _Shape { <--  one of the derived classes
 *   };
 *
 *   class Shape : public Wrapper<_Shape> {
 *   public:
 *     void draw(void) const { core->draw(); }
 *     void rotate (int x) { 
 *       clone(); <-- REQUIRED for non-const functions
 *       core->rotate(x);
 *   };
 *
 * And that's all there is to it.  By inheriting from Wrapper, you get
 * the "core" variable of type Base* (for you inheritance hierarchy).
 * Use this to make all the "obvious" one-liner member functions of
 * your new wrapper class.  Be sure to call clone() first in anything
 * that is not const.
 * 
 *
 * ###### RULES FOR USING THIS CLASS (read and follow all of these!)
 *
 * 1. The base class (class T) MUST PROVIDE A "virtual T* copy(void) const"
 * This function is the basis for the inherited copy constructor.
 * Every derived class of T should define it's own copy
 * class X : T {
 * public:
 *   T* copy(void) const { return new X(*this); }
 * };
 *
 * 2. Every method in the base class that is non-const must call clone()
 *
 * 3. Constructors are hard (this isn't a rule, but read on anyway)
 * Try this strategy:
 *   class Foo : public Wrapper<Bar> {
 *     static Bar* create(args...) { return new Bar(args...); }
 *   public:
 *     Foo(args...) : Wrapper<Bar>(create(args...)) {}
 *     Foo(void) : Wrapper<Bar>(new Bar) {}
 * };
 *
 */
template <class T>
class Wrapper {
public:
  typedef T* ptrT;
private:
  struct Cell {
    ptrT data;
    int refs;
  };
protected:
  /* if C++ allowed us to, cell and core would be readonly */
  Cell* cell;                    // do NOT modify in derived classes!!!
  ptrT core;                    // do NOT modify in derived classes!!!
public:
  Wrapper(ptrT p) { 
    core = p;
    cell = new Cell;
    cell->data = core;
    cell->refs = 1;
  }

  Wrapper(const Wrapper<T>& w) {
    ref(w);
  }

  /* NOTE: it is not safe to allow operator= to return a Wrapper<T>&
     Someone might implement hello world something like:

       class _Foo {
       public:
         operator int() { return 1; }
       };

       class Foo : public Wrapper<_Foo>{ 
       public:
         Foo(void) { core = new _Foo; }
         operator int() { return (int) (*core); }
       };

     and then

       main()
       { 
         Foo x, y;
         if (x = y) cout << "Hello World\n";
       }

     Note that if Foo::operator=() existed, then this code would work.
     But, since we only have Wrapper::operator=(), the type of (x = y) is
     Wrapper<T>&  and not Foo&

     Soooo.. Wrapper::operator=() returns void instead.  Probably a good
     thing.  People who program like

       if ((f = fopen("foo", "r")) == NULL) {

       }

     deserve what they get.
     */
  void operator=(const Wrapper<T>& w) {
    if (cell == w.cell) return; // already identical to each other
    unref();
    ref(w);
  }

  ~Wrapper(void) { 
    unref();
  }

  void clone(void) {
    if (cell->refs == 1) return;
    ptrT p = cell->data->copy();
    unref();
    cell = new Cell;
    cell->data = p;
    cell->refs = 1;
    core = p;
  }

private:
  void ref(const Wrapper<T>& w) {
    cell = w.cell;
    cell->refs += 1;
    core = cell->data;
  }

  void unref(void) {
    cell->refs -= 1;
    if (cell->refs == 0) { // last reference
      delete cell->data;
      delete cell;
    }
  }
};


/*
 * A Wrapper class that provides everything that Wrapper<T> does,
 * but also provides operator==
 * You must include <std/typeinfo.h> and also compile with -frtti (g++)
 *
 * equivalence is a tricky thing with inheritance hierarchies.
 * this wrapper class relies on RTTI and the existence of a method
 *   bool T::is_eq(const T*) const;
 *
 * If operator == is called for a wrapper around 'p' and a wrapper 
 * around 'q', then first I check to see if 'p' and 'q' are the same
 * type.
 */
template <class T>
class Wrapper_EQ : public Wrapper<T> {
public:
  Wrapper_EQ(T* p) : Wrapper<T>(p) {}
  bool operator==(const Wrapper_EQ<T>& w) const {
    if (typeid(*Wrapper<T>::cell->data) == typeid(*w.Wrapper<T>::cell->data)) 
      return Wrapper<T>::cell->data->is_eq(w.Wrapper<T>::cell->data);
    else
      return false;
  }
};


#endif /* !(_Wrapper_h) */
