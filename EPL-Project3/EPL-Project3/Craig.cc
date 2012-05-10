#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "String.h"
#include "ObjInfo.h"
#include "Set.h"
#include "Craig.h"
#include "Window.h"

using namespace std;

Initializer<Craig> __Craig_initializer;

String Craig::species_name(void) const
{
  return "Craig";
}

Action Craig::encounter(const ObjInfo& info)
{
  if (info.species == species_name()) {
    /* don't be cannibalistic */
    set_course(info.bearing + M_PI);
    return LIFEFORM_IGNORE;
  }
  else {
    Delete( hunt_event);
    hunt_event = new Event(0.0, make_procedure(*this, &Craig::hunt));
    return LIFEFORM_EAT;
  }
}

void Craig::initialize(void)
{
  LifeForm::add_creator(Craig::create, "Craig");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Craig::Craig()
{
  hunt_event = Nil<Event>();
  (void) new Event(0.0, make_procedure(*this, &Craig::live));
}


Craig::~Craig()
{
}

void Craig::spawn(void)
{
  Craig* child = new Craig;
  reproduce(child);
}


Color Craig::my_color(void) const
{
  return RED;
}  

LifeForm* Craig::create(void)
{
  Craig* res = Nil<Craig>();
  res = new Craig;
  res->display();
  return res;
}


/*
 * this event is only called once, when the object is "born"
 *
 * I don't want to call "set_course" and "set_speed" from the constructor
 * 'cause OBJECTS ARE NOT ALIVE WHEN THEY ARE CONSTRUCTED (remember!)
 * so... in the constructor I schedule this event to happen in 0 time
 * units
 */
void Craig::live(void)
{
  set_course(drand48() * 2.0 * M_PI);
  set_speed(2 + 5.0 * drand48());
  hunt_event = new Event(10.0, make_procedure(*this, &Craig::hunt));
}

void Craig::hunt(void)
{
  const String fav_food = "Algae";

  hunt_event = Nil<Event>();
  ObjList prey = perceive(20.0);

  double best_d = HUGE;
  for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
    if ((*i).species == fav_food) {
      if (best_d > (*i).distance) {
        set_course((*i).bearing);
        best_d = (*i).distance;
      }
    }
  }

  hunt_event = new Event(10.0, make_procedure(*this, &Craig::hunt));

  if (health() >= 4.0) spawn();
}

