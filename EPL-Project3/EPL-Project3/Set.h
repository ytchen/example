#if !(_Set_h)
#define _Set_h 1

#include <assert.h>

/* this is an inefficient implementation of a Set.  */

template <class T>
class Set {
#ifdef XLC_IS_STUPID
public:
#endif /* XLC_IS_STUPID */ 
  struct Link {
    T item;
    Link* next;
    Link* prev;
    Link(const T& x) : item(x) {
      next = prev = 0;
    }
    ~Link(void) {
      prev = 0;
      delete next;
    }
  };

  Link* head;

public:
  class Iterator {
  public:
    Link* p;
    Iterator(void) : p(0) {}
    Iterator(Link* x) : p(x) {}
    ~Iterator(void) {}
    Iterator operator++(void) {
      p = p->next;
      return *this;
    }
    Iterator operator++(int) { return operator++(); }

    T& operator*(void) {
      return p->item;
    }

    T* operator->(void) {
      return &p->item;
    }

    Iterator operator--(void) {
      p = p->prev;
      return *this;
    }
    Iterator operator--(int) { return operator--(); }

    bool operator==(const Iterator& i) const {
      return p == i.p;
    }
    bool operator!=(const Iterator& i) const {
      return p != i.p;
    }
  };

  Set(void) { head = 0; }
  Set(const Set<T>& s) { 
    head = 0;
    for (Iterator i = s.begin(); i != s.end(); ++i)
      insert(*i);
  }

  ~Set(void) { delete head; }

  Iterator begin(void) const { return Iterator(head); }
  Iterator end(void) const { return Iterator(0); }

  void insert(const T& x) {
    if (! is_member(x)) {
      Link* l = new Link(x);
      l->next = head;
      if (head) head->prev = l;
      l->prev = 0;
      head = l;
    }
  }

  void remove(const T& x) {
    Iterator i;
    if ((i = lookup(x)) != end())
      remove(i);
  }

  void remove(const Iterator& i) {
    Link* p = i.p;
    if (p->next) p->next->prev = p->prev;
    if (p->prev) p->prev->next = p->next;
    if (head == p) head = p->next;
    p->prev = p->next = 0;
    delete p;
  }

  Iterator lookup(const T& x) const {
    for (Iterator i = begin(); ! (i == end()); ++i)
      if (*i == x) return i;
    return Iterator(0);
  }

  bool is_member(const T& x) const {
    return end() != lookup(x);
  }

  /* union */
  Set<T>& operator|=(const Set<T>& s) {
    for (Iterator i = s.begin(); i != s.end(); ++i)
      insert(*i);
    return *this;
  }

  /* intersection */
  Set<T>& operator&=(const Set<T>& s) {
    for (Iterator i = begin(); i != end(); ++i)
      if (!s.is_member(*i)) remove(i);
    return *this;
  }

  /* assignment */
  Set<T>& operator=(const Set<T>& s) { 
    if (&s == this) return *this;
    delete head; head = 0;
    for (Iterator i = s.begin(); i != s.end(); ++i)
      insert(*i);
    return *this;
  }

  bool is_empty(void) const {
    return begin() == end();
  }
};

#endif /* !(_Set_h) */
