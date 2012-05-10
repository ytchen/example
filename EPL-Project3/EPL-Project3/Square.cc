#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "Params.h"
#include "Event.h"
#include "String.h"
#include "ObjInfo.h"
#include "Set.h"
#include "Square.h"
#include "Window.h"

using namespace std;

static Initializer<Square> __Square_initializer;

String Square::species_name(void) const
{
  return "Square";
}

Action Square::encounter(const ObjInfo&)
{
  return LIFEFORM_IGNORE;
}

void Square::initialize(void)
{
  LifeForm::add_creator(Square::create, "Square");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Square::Square(void)
{
  (void) new Event(0, 
		   make_procedure(*this, &Square::live));
}

void Square::live(void)
{
  set_speed(5.0);
  set_course(0.0);
  turn_event = new Event(5.0, 
                         make_procedure(*this, &Square::turn));
}

void Square::turn(void)
{
  set_course(get_course() + M_PI/2.0);
  turn_event = new Event(5.0, 
                         make_procedure(*this, &Square::turn));
}


Square::~Square(void)
{}


Color Square::my_color(void) const
{
  return BLUE;
}

LifeForm* Square::create(void)
{
  return new Square;
}

