#if ! (_LifeForm_h)
#define _LifeForm_h 1

#include <cassert>
#include <vector>
#include <map>
#include <algorithm>
#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif
#include "Params.h"
#include "Point.h"


/* forward declarations */
class LifeForm;
class istream;
struct ObjInfo;
template <class T> class Set;
typedef std::vector<ObjInfo> ObjList;
template <class Obj> class QuadTree;
template <class Obj> class TreeNode;

/* 
 * The map will contain IstreamCreators for LifeForms
 * The map will be keyed on a String.  This String
 * must be a keyword that uniquely identifies a type of LifeForm
 * (e.g., "Lion", "Tiger", "Bear", oh my).
 */
typedef LifeForm* (*IstreamCreator)(void);

typedef std::map<std::string, IstreamCreator> LFCreatorTable;

/*
 * The Canvas class is something we can draw on
 */
class Canvas;

/*
 * We draw with Colors
 */
#include "Color.h"

/*
 * The Region class represents the simulation space (2-dimensional)
 */
class Event;


#if ! defined(myDelete)
#define myDelete(e) { if (e!=NULL&&e->now()!=Event::now()) { Delete(e); }}
#endif

enum Action {
  LIFEFORM_IGNORE,
  LIFEFORM_EAT
};

class LifeForm {
  static LifeForm* all_life;    // a doubly-linked list of all life forms
  static QuadTree<LifeForm*> space;
  LifeForm* next_life;          // links for that list
  LifeForm* prev_life;
  static unsigned num_life;     // length of the list

  double energy;
  bool is_alive;

  Event* border_cross_event;    // pointer to the event for the next
                                // encounter with a boundary

  Point pos;

  double update_time;           // the time when update_position was 
                                //   last called
  double reproduce_time;        // the time when reproduce was last called

  double course;
  double speed;
  
  Point start_point;

// priviate methods -- done
  static LFCreatorTable& istream_creators(void);  // done
  int scale_x(double) const;  // done
  int scale_y(double) const;  // done
  void print_position(void) const;
  void print(void) const;
  void die(bool=true);          // life form must be in the quad tree.

  /* !!! WARNING methods die and _die delete 'this'
     use with EXTREME care */
  void _die(void);
  const Point& position() const { return pos; }

// private methods -- just finished
  void age(void);               // subtract age_penalty from energy 
  void gain_energy(double);
  void eat(LifeForm*);
  ObjInfo info_about_them(const LifeForm*) const;
  void compute_next_move(void);
  void update_position(void);   // calculate the current position for
                                // an object.  If less than Time::tolerance
                                // time units have passed since the last
                                // call to update_position, then do nothing
                                // (we can't have moved very far so there's
                                // no point in updating our position)
  void check_encounter(void);   // check to see if there's another object
                                // within encounter_distance.  If there's
                                // an object nearby, invoke resolve_encounter
                                // on ourself with the closest object
  void resolve_encounter(LifeForm*);
  void region_resize(void);
  void border_cross(void);
  


// private methods -- unfinished
  






 

protected:
  static Canvas win;
  double health(void) const {
    return energy / start_energy;
  }
  double get_course(void) const { return course; }
  double get_speed(void) const { return speed; }

// protected methods -- just finished
  void set_course(double);
  void set_speed(double);
  void reproduce(LifeForm*);
  ObjList perceive(double);

// protected methods -- unfinished

  
  





public:
  LifeForm(void);
  virtual ~LifeForm(void);

  static void add_creator(IstreamCreator, const std::string&);
  static void create_life();
  /* draw the lifeform on 'win' where x,y is upper left corner */
  virtual void draw(int, int) const;
  

  void display(void) const;
  static void redisplay_all(void);
  static void clear_screen(void);


// public methods -- just finished
  virtual std::string player_name(void) const;

// public methods -- unfinished
  virtual Color my_color(void) const =0;
  virtual Action encounter(const ObjInfo&) =0;
  virtual std::string species_name(void) const = 0;
  

friend class Algae;

/*
 * the following functions are used for testing only and should not be used by students (except, of course,
 * during testing)
 */
	bool confirmPosition(double xpos, double ypos) {
		return pos.distance(Point(xpos,ypos)) < 0.10;
	}

	static void runTests();
	static bool testMode;
	static void place(LifeForm*, Point p);

};

#endif /* !(_LifeForm_h) */
