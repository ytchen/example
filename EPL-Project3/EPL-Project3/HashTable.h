#if !(_HashTable_h)
#define _HashTable_h 1

#if defined(_AIX) && !defined(XLC_IS_STUPID) && !defined(__GNUG__)
#define XLC_IS_STUPID
typedef int bool;
const bool false = 0;
const bool true = 1;
#endif /* _AIX */

#include <utility>
#include <vector>
#include "Wrapper.h"

using std::pair;

template <class T, class Key, class HashFun> 
class HashTable {
  typedef pair<Key, T> Entry;   // we store these objects in the table

  typedef vector<Entry> Bucket;   // A 'Bucket' is a linked list
                                // implemented with Cons cells.
private:
  Bucket* bucket;
  unsigned size;                // number of buckets in the hash table
                                // size must be smaller than MAX_SIZE

  static const unsigned MAX_SIZE 
  = 1000000; // 1 million seems pretty large to me.

public:
  HashTable(unsigned sz = 128) {
    assert(sz < MAX_SIZE);
    size = sz;
    bucket = new Bucket[size];
  }

  ~HashTable(void) {
    delete[] bucket;
  }

  /* insert a copy of 'x' into the hash table */
  void insert(const T& x, const Key& key) {
    unsigned b = HashFun()(key) % size;
    bucket[b].push_back(Entry(key,x));
  }

  bool is_member(const Key& key) {
    return lookup(key) != end();
  }

public:
  class _iterator {
  public:
    virtual Key& deref_key(void) = 0;
    virtual T& deref(void) = 0;
    virtual _iterator* copy(void) const = 0;
    virtual void increment(void) = 0;
    virtual bool is_end(void) const = 0;
    virtual ~_iterator(void) {}
  };

  /* the iterators are friends so that they can have access to the
     nested type, "Bucket" */
  friend class iterator;
  friend class _iterator;
  friend class BasicIterator;
  friend class KeyedIterator;


  class iterator : public Wrapper<_iterator> {
  public:
    iterator(const typename Bucket::iterator& b, 
             const typename Bucket::iterator& e,
             const Key& k) 
      : Wrapper<_iterator>(new HashTable<T,Key,HashFun>::KeyedIterator(b,e,k)) 
    {}

    iterator(Bucket* bucket, unsigned size)
    : Wrapper<_iterator>(new 
                         HashTable<T,Key,HashFun>::BasicIterator(bucket, size))
    {}

    iterator(void) : Wrapper<_iterator>(new BasicIterator(0, 0)) {}

    Key& key(void) { return core->deref_key(); }
    T& operator*(void) { return core->deref(); }

    iterator& operator++(void) { 
      clone();
      core->increment();
      return *this;
    }

    iterator operator++(int) {
      clone();
      iterator t = *this;
      operator++();
      return t;
    }

    bool operator==(const iterator& p) const {
      /* if both iterators are at end, return true */
      if (core->is_end() && p.core->is_end()) return true;
      /* if neither iterator is at end, then compare the address
         of the object they point to */
      else if ((!core->is_end()) && !(p.core->is_end()))
        return (&core->deref() == &(p.core->deref()));
      /* otherwise, one must be end, and the other not at end */
      else return false;
    }

    bool operator!=(const iterator& p) const {
      return !(*this == p);
    }
  };

  /* The Iterator iterates only through the elements that
     match the key.  lookup(const Key&) returns a Iterator */
  class KeyedIterator : public _iterator {
    typename Bucket::iterator current;
    typename Bucket::iterator end;
    Key key;
  public:
    KeyedIterator(const typename Bucket::iterator& b, 
                  const typename Bucket::iterator& e, 
                  const Key& k) {
      current = b;
      end = e;
      key = k;
    }
    ~KeyedIterator(void) {}
    void increment(void) {
      if (current != end) current++;
      while(current != end && (*current).first != key)
        current++;
    }
    T& deref(void) { return (*current).second; }
    Key& deref_key(void) { return (*current).first; }

    bool is_end(void) const { return current == end; }

    _iterator* copy(void) const { 
      return new HashTable<T,Key,HashFun>::KeyedIterator(*this); 
    }


  private:
    /* this copy constructor is used in 'copy()', but should
       otherwise not be used */
    KeyedIterator(const KeyedIterator& p) { 
      current = p.current;
      end = p.end;
      key = p.key;
    }

    /* assignment should not ever be used */
    void operator=(const KeyedIterator&) {
      assert(0);
    }
  };

  class BasicIterator : public _iterator {
    Bucket* bucket;       // an array Bucket[0..size-1]
    unsigned size;
    unsigned bkt;               // current and end point at bucket[bkt] 
    typename Bucket::iterator current;   // current pointer for the iterator
    typename Bucket::iterator end;       // end of bucket[bkt]

    /* DESIGN NOTES:
     *  1. The implementation *MUST* maintain the invariant that
     *      current != end
     *  except when 'is_end()' is true
     *
     *  2. it is not safe to dereference 'current' when is_end() is true
     *     this is generally true of all iterators
     */

    /* Advance until we find the next non-empty bucket */
    void find_next(void) {
      while (bkt < size && current == end) {
        bkt += 1;
        if (bkt < size) {
          current = bucket[bkt].begin();
          end = bucket[bkt].end();
        }
      }
    }

  public:
    /* Design NOTE:
     *  It is not safe to dereference 'b' if size is 0
     *  More precisely, 'bucket[k]' is a legal expression only if k
     *  is strictly less than size
     */
    BasicIterator(Bucket* b, unsigned s) {
      bucket = b;
      size = s;
      bkt = 0;
      if (bkt < size) {         // No-arg Iterator constructor creates
                                // a Basic iterator with a bogus
                                // Bucket ptr.  end() does too.
                                // (see Design Notes)
        current = b[0].begin();
        end = b[0].end();
        find_next();
      }
    }

    ~BasicIterator(void) {}
    void increment(void) {
      if (is_end()) return;
      else {
        current++;
        if (current == end) find_next();
      }
    }

    Key& deref_key(void) { return (*current).first; }
    T& deref(void) { return (*current).second; }

    bool is_end(void) const { 
      return bkt == size;
    }

    _iterator* copy(void) const { 
      return new HashTable<T,Key,HashFun>::BasicIterator(*this); 
    }

  private:
    /* this copy constructor is used in 'copy()', but should
       otherwise not be used */
    BasicIterator(const BasicIterator& p) { 
      bucket = p.bucket;
      size = p.size;
      bkt = p.bkt;
      current = p.current;
      end = p.end;
    }

    /* assignment should not ever be used */
    void operator=(const BasicIterator&) {
      assert(0);
    }
  };

  /* return some matching entry (if there are multiple matches
     then, which one is returned is implementation dependent) */
  iterator lookup(const Key& key) const {
    int b = HashFun()(key) % size;
    typename Bucket::iterator p = bucket[b].begin();
    typename Bucket::iterator e = bucket[b].end();
    while (p != e && (*p).first != key)
      p++;

    return HashTable<T,Key,HashFun>::iterator(p, e, key); 
  }

  iterator begin(void) { 
    return HashTable<T,Key,HashFun>::iterator(bucket, size); 
  }

  iterator end(void) { 
    return HashTable<T,Key,HashFun>::iterator(bucket, 0); 
  }

private:
  /* these next couple are place holders.... If we need them,
     we can implement real methods later.... but I want to protect
     myself from the automatic compiler-generated methods */
  
  /* don't copy or assign */
  HashTable(const HashTable<T, Key, HashFun>&) { assert(0); } 
  HashTable<T,Key,HashFun>& operator=(const HashTable<T, Key, HashFun>&) {   
    assert(0); return *this;
  } 

};


#endif /* !(_HashTable_h) */
