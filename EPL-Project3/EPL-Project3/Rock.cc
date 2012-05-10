#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "Params.h"
#include "Event.h"
#include "String.h"
#include "ObjInfo.h"
#include "Set.h"
#include "Rock.h"
#include "Window.h"

using namespace std;

static Initializer<Rock> __Rock_initializer;

String Rock::species_name(void) const
{
  return "Rock";
}

Action Rock::encounter(const ObjInfo&)
{
  return LIFEFORM_IGNORE;
}

void Rock::initialize(void)
{
  LifeForm::add_creator(Rock::create, "Rock");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Rock::Rock(void)
{
  /* rocks are kinda boring */
}

Rock::~Rock(void)
{}


Color Rock::my_color(void) const
{
  return ORANGE;
}

LifeForm* Rock::create(void)
{
  return new Rock;
}

