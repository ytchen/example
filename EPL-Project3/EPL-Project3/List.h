/* 
 * $Id: List.h,v 1.3 1997/02/14 23:52:32 chase Exp $
 *
 * Copyright C. M. Chase 1994, 1995, 1996
 *
 * Permission is granted to use this code in any way.
 * This code is provided as is, with no warranty expressed or implied
 * or warranty for mechantability or fitness for a particular purpose.
 * Use at your own risk.  This software may contain defects which could
 * cause it to malfunction in possibly dangerous ways.
 *  
 * Neither the author, nor any affiliates of the author are responsible for 
 * any damages caused by use of this software.
 *
 */

/*
 * Description:
 *
 * Doubly linked circular list
 * The Deque is maintained as a set of copies of objects 
 * i.e.
 *    Deque<int> d;
 *    int x = 5;
 *    d.insert(x);
 *    x = 3;
 *    cout << ddl.top();
 * will print "5"
 *
 * NOTE: you can only put objects into an List that have a equivalence
 * operator (==) defined for them.  This operator is used to check
 * for membership in the lists.
 * i.e. 
 *     bool operator ==(T& x, T& y);  <-- must be defined
 *
 * If you don't have an equivalence operator, just use a Deque
 *
 *
 * This container should be flexible enough to implement FIFO, or LIFO
 * stacks and queues, as well as standard lists.
 *
 * A simple iterator is defined, use with care.  It roughly satisfies
 * the requirments for a bi-directional iterator.  There is no
 * const_iterator defined.  You are advised to avoid postincrement and
 * postdecrement operations.  Although the usual for loop will work
 *   "for (p = l.begin(); p != l.end() p++)"
 * tricky things like "++(p++)" will *not* work.  Personally, if you 
 * program like this, you probably deserve what you get, but
 * that's just my opinion.
 *
 */   

#ifndef _List_h
#define _List_h

template <class T>
class Cell {
public:
  T item;
  Cell<T>* next;
  Cell<T>* prev;

  Cell(const T& x) {
    next = prev = 0;
    item = x;
  }

  Cell() {
    next = prev = 0;
  }

  ~Cell() {}

private: // don't use unless we have to.
  Cell(const Cell<T>& d) {
    next = prev = 0;
    item = d.item;
  }

  Cell<T>& operator=(const Cell<T>& d) {
    next = prev = 0;
    item = d.item;
    return *this;
  }

};


/*
  Doubly linked circular list
  Objects inserted into the list are automatically copied.  A copy is
  kept in the list.  If the original object is subsequently changed, the 
  copy in the list is not affected.
  
   This container should be flexible enough to implement FIFO, or LIFO
   stacks and queues, as well as standard lists.
*/


template <class T> 
class Deque {
public:
  typedef Cell<T>* cell_ptr;
protected:

  cell_ptr dummy;
  cell_ptr& head_cell() const { return dummy->next; }
  cell_ptr& tail_cell() const { return dummy->prev; }

  void unlink(cell_ptr cp) {    // delete a cell from the list
    cp->next->prev = cp->prev;
    cp->prev->next = cp->next;
  }

public:
  Deque(void) {
    dummy = new Cell<T>;
    dummy->next = dummy;
    dummy->prev = dummy;
  }

  Deque(const Deque<T>& l) {
    dummy = new Cell<T>;
    dummy->next = dummy; dummy->prev = dummy;
    cell_ptr p = l.head_cell();
    while (p != l.dummy) {
      insert_tail(p->item);
      p = p->next;
    }
  }

  Deque<T>& operator=(const Deque<T>& l) {
    if (&l == this) return *this; // protect against "x = x";

    clean();                    // remove prev contents
    
    cell_ptr p = l.head_cell(); // copy contents from l
    while (p != l.dummy) {
      insert_tail(p->item);
      p = p->next;
    }

    return *this;
  }

  ~Deque() {
    clean();
    delete dummy;
  }

  class Iterator {
    cell_ptr p;
  public:
    Iterator(void) {
      p = 0;
    }

    Iterator(const Iterator& x) {
      p = x.p;
    }

    bool operator==(const Iterator& x) const {
      return this->p == x.p;
    }

    bool operator!=(const Iterator& x) const {
      return ! operator==(x);
    }

    Iterator(const cell_ptr& x) {
      p = x;
    }

    Iterator& operator++(void) { // pre-increment
      p = p->next;
      return *this;
    }

    Iterator operator++(int) { // post-increment
      Iterator t = *this;
      operator++();
      return t;
    }
      
    Iterator& operator--(void) { // pre-decrement
      p = p->prev;
      return *this;
    }

    Iterator operator--(int) { // post-decrement
      Iterator t = *this;
      operator--();
      return t;
    }

    T& operator*(void) { // dereference
      return p->item;
    }

    T* operator->(void) { // member deref
      return &p->item;
    }
    
    Deque<T>::Iterator& operator=(const Deque<T>::Iterator& x) {
      p = x.p;
      return *this;
    }

    friend class Deque<T>;   // List needs to access Iterator::p in order 
                                // to implement insert(T, Iterator)
                                // and remove(Iterator)
  };

  Iterator begin() {
    return Iterator(head_cell());
  }

  Iterator end() {
    return Iterator(dummy);
  }
  
  bool is_empty() const {
    return head_cell() == dummy;
  }


  T top(void) const {
    assert(!is_empty()); // "Attempt to take top of empty list"
    return head_cell()->item;
  }

  T bottom(void) const {
    assert(!is_empty()); // "Attempt to take bottom of empty list";
    return tail_cell()->item;
  }

  unsigned length() const {     // slow
    int len = 0;
    for (cell_ptr p = head_cell(); p != dummy; p = p->next)
      len += 1;
    return len;
  }

  /* insert a new item before the current head */
  T insert_head(const T& x) {
    cell_ptr cell = new Cell<T>(x);
    cell->next = head_cell();
    cell->prev = (head_cell())->prev;   
    (head_cell())->prev = cell;
    head_cell() = cell;
    return cell->item;
  }

  /* insert a new item after the current tail */
  T insert_tail(const T& x) {
    cell_ptr cell = new Cell<T>(x);
    cell->next = tail_cell()->next;
    cell->prev = tail_cell();
    (tail_cell())->next = cell;
    tail_cell() = cell;
    return cell->item;
  }

  T insert(const T& x) { return insert_tail(x); }

  /* insert 'x' before position 'i' */
  T insert(const T& x, const Deque<T>::Iterator& i) {
    cell_ptr succ = i.p;
    cell_ptr cell = new Cell<T>(x);
    cell->next = succ;
    cell->prev = succ->prev;
    succ->prev->next = cell;
    succ->prev = cell;
    return cell->item;
  }

  T remove_head(void) {
    assert(!is_empty());
    cell_ptr cp = head_cell();
    T y = cp->item;
    unlink(cp);
    delete cp;
    return y;
  }

  T remove_tail(void) {
    assert(!is_empty());
    cell_ptr cp = tail_cell();
    T& y = cp->item;
    unlink(cp);
    delete cp;
    return y;
  }

  /* default behavior is a FIFO queue */
  T remove(void) { return remove_head(); }

  
  /* remove (and return) the item at position 'i' in the list */
  T remove(const Deque<T>::Iterator& i) {
    cell_ptr data_cell = i.p;
    assert(data_cell != dummy); 
    T y = data_cell->item;
    unlink(data_cell);
    delete data_cell;
    return y;
  }

  void clean() {
    cell_ptr cp = head_cell();
    cell_ptr next_cp;
    while (cp != dummy) {
      next_cp = cp->next;
      unlink(cp);
      delete cp;
      cp = next_cp;
    }
  }

};


template <class T> 
class List : public Deque<T> {

  cell_ptr member_cell(const T& x) const { // search for a cell
    cell_ptr curr_cell = head_cell();
    while (curr_cell != dummy) {
      if (curr_cell->item == x)
        return curr_cell;
      curr_cell = curr_cell->next;
    }
    return (cell_ptr) 0;
  }


public:
  List(void) {}
  List(const List<T>& l) : Deque<T>(l) { }

  ~List() {
    /* the Deque Destructor is all we need */
  }


  bool is_member(const T& x) const {
    if (member_cell(x))
      return true;
    else
      return false;
  }
  

  /* extract an item from a list.  ITEM MUST BE IN THE LIST!!! */ 
  T remove(const T& x) {
    cell_ptr data_cell = member_cell(x);
    assert(data_cell);
    unlink(data_cell);
    T y = data_cell->item;
    delete data_cell;
    return y;
  }

  /* IMPLEMENTATION NOTE: 
   * I learned a subtle feature of C++ (documented bug?) while writing
   * this code.  One cannot overload existing methods from the base class
   * by writing methods with the same name in the derived class.
   * For example, the "remove(const T&)" method above causes List<T>
   * to stop inheriting remove(void) and remove(const Iterator&)
   * So... in order to inherit these methods, I need to create wrapper
   * methods in the derived class for each method in the base class.
   * This really sucks, since any change in the base class would not
   * propagate to the derived class
   *
   * The only other solution is to change "remove(const T&)" to some other
   * name, like "remove_copy(const T&)"
   * Kinda defeats the purpose of overloading the function "remove" if 
   * you ask me.
   */
  T remove(void) {
    return Deque<T>::remove();
  }

  T remove(const Deque<T>::Iterator& i) {
    return Deque<T>::remove(i);
  }

  /* discard an item from a list.  If the item is in the list
     return true, otherwise, return false */
  bool discard(const T& x) {
    cell_ptr data_cell = member_cell(x);
    if (data_cell) {
      unlink(data_cell);
      delete data_cell;
      return true;
    }
    else
      return false;
  }
};


#endif /* _List_h */
