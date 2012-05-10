#include <iostream>
#include <cstdlib>

#include <math.h>
#include "Params.h"
//#include "TestResults.h"
#include "TestForm.h"
#include "QuadTree.h"
#include "Window.h"
#include "ObjInfo.h"
#include "LifeForm.h"


using namespace std;

double eat_success = 1.0;
double age_frequency = 1.0e9;
bool g_use_algae_spores = false;

#define TESTDEBUG 1


/*------------------------------------------------------------------------------*/

bool _close_counts(double a, double b, double fudge /* = VERY_CLOSE */)
{
   if (a >= b) return (a - b < fudge);
   return (b - a < fudge);
}



/*------------------------------------------------------------------------------*/
// TestForm suport methods


bool TestForm::_lived_too_long = false;

/**********************************************************************
 * Good for creating an event to check your energy level in the future.
 * @param   expected_value  what energy should be
 * @param   health_ok  boolean variable to set to false
 *          if energy is not equal to expected_value
 */
void TestForm::check_health(double expected_value, string check_for, bool* health_ok)
{
   assert(health_ok != NULL);

   if (! _close_counts(energy, expected_value) )
   {  *health_ok = false;
#if TESTDEBUG
      cerr << " DRAT:  check_health() for " << check_for << ": expected energy = "
            << expected_value << ", but got energy = " << energy << endl;
#endif
   }
#if TESTDEBUG
   else
   {  *health_ok = true;
      cerr << "      check_health() for " << check_for << ": energy level "
            << energy << " looks good" << endl;
   }
#endif

   if (energy < min_energy && !_close_counts(energy, min_energy))
   {  cerr << " DRAT:  my energy is " << energy << " and I'm still alive!" << endl;
      _lived_too_long = true;
      die();
   }
}


//
// Protected Methods.
//

// make sure rads is between 0 and 2*M_PI
double TestForm::_normalize_angle(double rads) const
{
   bool reverse = false;
   if (rads < 0.0)
   {  rads = -rads;
      reverse = true;
   }

   double multi = rads / (2*M_PI);
   rads -= ( ((double)((long)multi)) * (2*M_PI) );
   if (rads > (2*M_PI)) rads = 0.0;    // for floating point precision variance

   if (reverse)
   {  rads = -rads;
   }

   return rads;
}


/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*---------------                                --------------*/
/*---------------    DERIVED CLASSES BELOW       --------------*/
/*---------------                                --------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/



/*------------------------------------------------------------------------------*/
// Test 1

/*** Static data. */

RkInitializer<Test1> __Test1_initializer("Test1");
T1Results Test1::_tr;


/*** From base class. */

LifeForm* Test1::create(void)
{
	cout << "Test 1 create\n";
   Test1* t1 = new Test1;
   t1->pos.xpos = grid_max / 2.0;
   t1->pos.ypos = grid_max / 2.0;

   return t1;
}


void Test1::_lifeform_start(void)
{
   cout << "perceive():\n";
   cout.flush();
   schedule<Test1>(1.0, self, &Test1::_doPerceive);
}


/*** Test1-specific. */

Test1::~Test1()
{
   if (energy >= min_energy)
   {
#if TESTDEBUG
      cerr << " DRAT:  Died when energy >= min_energy!" << endl;
#endif
   }
   else
   {
      _tr.iDied();
   }

   schedule<TestResults>(0.0, _tr, &TestResults::endTest);
}

void Test1::_doPerceive()
{
   bool bFirstTime = _tr.firstTime();
   cerr << "        calling perceive" << endl;

   if (bFirstTime)
   {
      // following value should allow _tr.expectedPerceives() perceive() calls, and
      // I shoul die on the last one (or very shortly thereafter).
      energy = ((double)_tr.expectedPerceives() * perceive_cost(max_perceive_range))
               + min_energy - 1.0;
#if TESTDEBUG
      cerr << "        Starting with energy " << energy << endl;
#endif
   }

   double energy_before = energy;
   _tr.incrementPerceives();
   ObjList set = perceive(max_perceive_range);

   if (bFirstTime)
   {
      int number = 0;
      ObjList::iterator it = set.begin();
      while(it != set.end())
      {
         number++;
         Point other_pos;
         other_pos.xpos = pos.xpos + ((*it).distance * cos((*it).bearing));
         other_pos.ypos = pos.ypos + ((*it).distance * sin((*it).bearing));
         if (! space.is_occupied(other_pos))
         {
            _tr.badBearingsDist();
#if TESTDEBUG
            cerr << " DRAT:  perceive() reports a " << (*it).species << " at ("
                  << other_pos.xpos << "," << other_pos.ypos
                  << "), but there is none there." << endl;
#endif
         }
#if TESTDEBUG
         else
         {  cerr << "        found a " << (*it).species << " at ("
                  << other_pos.xpos << "," << other_pos.ypos
                  << "), right where perceive() reported one!" << endl;
         }
#endif

         ++it;
      }

      // Test 1 is run with one Test1 object and four PlacedRock objects.
      if (number != 4)
      {
         _tr.badPerceive();
#if TESTDEBUG
         cerr << " DRAT:  perceive() did not return 4 objects" << endl;
#endif
      }
      else
      {
         _tr.goodPerceive();
#if TESTDEBUG
         cerr << "        perceive() returned 4 objects, as expected" << endl;
#endif
      }

      double exp = energy_before - perceive_cost(max_perceive_range);
      string str("perceive cost");
      schedule<Test1,double,double>(0.1, self, &Test1::_checkEnergy, energy_before, exp);

      // in case I don't die like I'm supposed to
      schedule<TestResults,string,bool>(5.0, _tr,
                                        &TestResults::failedToDie,
                                          string("perceiving life form"),
                                          true);
   }

   schedule(0.2, self, &Test1::_doPerceive);
}



void Test1::_checkEnergy(double prev_energy, double expected)
{
   if (energy < prev_energy && ! _close_counts(energy, prev_energy))
   {
      _tr.penaltyAssessed(true);
      _tr.penaltyOk( _close_counts(energy, expected) );
   }
   else
   {
      _tr.penaltyAssessed(false);
      _tr.penaltyOk(false);
   }
}



/*------------------------------------------------------------------------------*/
// Test 2

/*** Static data */

RkInitializer<Test2> __Test2_initializer("Test2");
Test2* Test2::_pParent = NULL;
bool Test2::_bDidChildTestsAlready = false;
T2Results Test2::_tr;


/*** From base class */

LifeForm* Test2::create(void)
{
   Test2* t2 = new Test2;
   t2->pos.xpos = grid_max / 2.0;
   t2->pos.ypos = grid_max / 2.0;

   return t2;
}


void Test2::_lifeform_start(void)
{
   set_speed(0.0);
   _bEverLived = true;

   if (_tr.firstTime())
   {
      /*** Parent Test2 life form */
      _pParent = this;
      cout << "reproduce() and aging:\n";
      cout.flush();
      schedule(2.0, self, &Test2::_doReproduce);

      // following value allows one call to reproduce() without dying, and then
      // I should die the second time.
      energy = 3.0 * min_energy;
      _tr.setStartEnergy(energy);

      // After the reproduce() in 2.0 time units from now, I will reproduce
      // again 11.0 time units after that, at which point I should die. Thus,
      // I should die 13.0 time units in now's future.
      _pFailedToDie = schedule<TestResults,string,bool>(14.0,
                                                        _tr,
                                                        &TestResults::failedToDie,
                                                            string("parent life form"),
                                                            false);
      _failedToDieTime = Event::now() + 14.0;
   }
   else
   {
      /*** baby Test2 life form */
#if TESTDEBUG
      cerr << "        child created: " << this << endl;
#endif
      _tr.childCreated();

      /*** First, check energy levels of parent and child ater the reproduce */
      if (! _bDidChildTestsAlready)
      {
                              // first child only runs this code
         double startEnergy = _tr.startEnergy();
         double expected = ( (startEnergy / 2.0) * (1.0 - reproduce_cost) );
         if (energy < startEnergy && ! _close_counts(energy, startEnergy))
         {
            _tr.childEnergy(true, _close_counts(energy, expected));
         }
         else _tr.childEnergy(false, false);
#if TESTDEBUG
            cerr << "        child energy - expected " << expected
                  << ": got " << energy << endl;
#endif

         if (_pParent->energy < startEnergy && ! _close_counts(_pParent->energy, startEnergy))
         {
            _tr.parentEnergy(true, _close_counts(_pParent->energy, expected));
         }
         else _tr.parentEnergy(false, false);
#if TESTDEBUG
            cerr << "        parent energy - expected " << expected
                  << ": got " << energy << endl;
#endif


         /*** Next, check that the child is within reproduce_dist units of its parent */
         _checkDist();


         /*** Now, check for proper aging of the child
            * (the parent should die from trying to reproduce again)
            */
         _bCheckAging = true;
         energy = (age_penalty * 2.0) + min_energy - 1.0;      // die after 2nd age event
         double expected_aged_energy = energy - age_penalty;
#if TESTDEBUG
         cerr << "        start child aging sequence with energy = " << energy << endl;
#endif

         (void)schedule<Test2,double,double>(age_frequency + 1.0,
                                             self,
                                             &Test2::_checkAgingEnergy,
                                                energy,
                                                expected_aged_energy);
         
         // make sure I die after aging twice
         _pFailedToDie = schedule<TestResults,string,bool>((age_frequency * 2.0) + 10.0,
                                                           _tr,
                                                           &TestResults::failedToDie,
                                                               string("child life form"),
                                                               true);
      _failedToDieTime = Event::now() + (age_frequency * 2.0) + 10.0;

         _bDidChildTestsAlready = true;
	  } // if this is first child
#if TESTDEBUG
      else
      {
         cerr << " DRAT:  Uh oh.  Looks like dying parent was able to reproduce and\n"
            <<   "           create a child with energy = " << energy;
         cerr << ".  child committing suicide" << endl;
         is_alive = true;
         die(!space.is_out_of_bounds(pos) && space.is_occupied(pos));
      }
#endif

   }
}



/*** Test2-specific */

void Test2::_doReproduce()
{
   Test2* child = new Test2();
   reproduce(child);

// jmg  schedule(11.0, self, &Test2::Test2::_doReproduce);
		
	    //schedule(11.0, self, &Test2::Test2::_doReproduce);
			schedule(11.0,self,&Test2::_doReproduce);
}


void Test2::_checkDist()
{
   double dist = position().distance(_pParent->position());
   if (dist > reproduce_dist && ! _close_counts(dist, reproduce_dist))
   {
      _tr.childDistanceGood(false);
#if TESTDEBUG
      cerr << " DRAT:  child Test2 supposed to be within " << reproduce_dist
            << " units from its parent.  But I'm " << dist << " units away." << endl;
#endif
   }
   else
   {
      _tr.childDistanceGood(true);
#if TESTDEBUG
      cerr << "        child placed " << dist
            << " units from its parent - that's good" << endl;
#endif
   }
}


Test2::~Test2()
{
   if (_pFailedToDie != NULL
       && Event::now() < _failedToDieTime
       && !_close_counts(Event::now(), _failedToDieTime)
      )
   {
      delete _pFailedToDie;
      _pFailedToDie = NULL;
   }

   if (_bEverLived)
   {
      double now = Event::now();
      if (this == _pParent)
      {
#if TESTDEBUG
         /*** parent life form dying here */
         cerr << "        parent " << this << " dying at time " << now << endl;
#endif
      }
      else
      {
#if TESTDEBUG
         cerr << "        child " << this << " dying at time " << now << endl;
#endif
         if (_bCheckAging)
         {
            /*** baby life form dying here */
            double approx_expected_time = age_frequency * 2.0;

            _tr.logResult("child life form aged to death",      // from aging I hope
                      Event::now() >= approx_expected_time && now < approx_expected_time + 20.0);

            // end the bloody test
            schedule<TestResults>(0.0, _tr, &TestResults::endTest);
         }

      } // if _pParent - else

   } // if I ever lived (i.e., if I was not aborted in LifeForm::reproduce()

}



void Test2::_checkAgingEnergy(double prev_energy, double expected)
{
#if TESTDEBUG
   bool bOk;
#endif

   if (energy < prev_energy && ! _close_counts(energy, prev_energy))
   {
      if (_close_counts(energy, expected))
      {  _tr.childAgingOk(true, true);
#if TESTDEBUG
         bOk = true;
#endif
      }
      else
      {  _tr.childAgingOk(true, false);
#if TESTDEBUG
         bOk = false;
#endif
      }
   }
   else
   {  _tr.childAgingOk(false, false);
#if TESTDEBUG
      bOk = false;
#endif
   }

#if TESTDEBUG
   if (!bOk)
   {
      cerr << " DRAT:  aging - expected energy " << expected
         << " but I've got " << energy << endl;
   }
#endif
}




/*------------------------------------------------------------------------------*/
// Test 3

/*** Static data */

RkInitializer<Test3> __Test3_initializer("Test3");
T3Results Test3::_tr;

Test3* Test3::_pMover = NULL;
Test3* Test3::_pStatue = NULL;
double Test3::_startEnergy = 300.0;
bool Test3::_bStatueDead = false;


/*** From base class */

LifeForm* Test3::create(void)
{
   Test3* t3 = new Test3();
   placeObject(t3);

   return t3;
}

void Test3::placeObject(Test3* pLife)
{
   if (_tr.firstTime())
   {        // statue life form
      _pStatue = pLife;
      pLife->pos.xpos = (grid_max / 2.0) + 0.7;
      pLife->pos.ypos = grid_max / 3.0;
   }
   else
   {        // mover life form
      _pMover = pLife;
      pLife->pos.xpos = (grid_max / 2.0) - 10.0;   // approach _pStatue from approximately
      pLife->pos.ypos = (grid_max / 3.0) - 5.0;    // at about a 25-degree angle
   }
}


void Test3::_lifeform_start(void)
{
   energy = _startEnergy;     // make sure no one dies when they're not supposed to.
   if (this == _pStatue)
   {
      _printTestTitle();
      set_speed(0.0);
#if TESTDEBUG
      cerr << "        Statue starting with energy " << energy << endl;
#endif
   }
   else
   {
#if TESTDEBUG
      cerr << "        Mover starting with energy " << energy << endl;
#endif
      set_course( position().Point::bearing(_pStatue->position()) );
      set_speed(5.0);
      _startMoveTime = Event::now();

      double dist = position().distance(_pStatue->position());
      double delta = (dist + 10.0) / 5.0;
      _pPassStatueEvent = schedule<T3Results,bool,bool>(delta,
                                                        _tr,
                                                        &T3Results::reachedOrPassedStatue,
                                                            false,
                                                            missedEncounterHalt());

      if (! missedEncounterHalt())
      {
         _pFinishAfterPassed = schedule<Test3>(delta + 0.1, self, &Test3::_finishAfterNoEncounter);
      }
   }
}



/*** Test3-specific */

Action Test3::encounter(const ObjInfo& obj_info)
{
   if (this == _pMover)
   {
      _tr.reachedOrPassedStatue(true, false);
      if (_pPassStatueEvent != NULL)
      {  unschedule(_pPassStatueEvent);
      }
      if (_pFinishAfterPassed != NULL)
      {  unschedule(_pFinishAfterPassed);
      }
#if TESTDEBUG
      cerr << "        encounter with the _pStatue life form" << endl;
#endif
   }

   set_speed(0.0);


   /*** Figure energy stuff */
   double move_time = Event::now() - _startMoveTime;
   double expected_movement_cost = movement_cost(5.0, move_time);

   
   // this event will check for correct energy and setup a check for digestion
   // event for the _pMover object.
   (void)schedule<Test3,double>(0.1,
                          self,
                          &Test3::_afterEncounter,
                              (this == _pMover ? expected_movement_cost : 0.0));

   Action action = LIFEFORM_IGNORE;
   if (this == _pMover) action = encounterAction();

   return action;
}


void Test3::_finishAfterNoEncounter()
{
#if TESTDEBUG
      cerr << "        _no_ encounter with the _pStatue life form" << endl;
#endif

   set_speed(0.0);


   /*** Figure energy stuff */
   double move_time = Event::now() - _startMoveTime;
   double expected_movement_cost = movement_cost(5.0, move_time);

 
   // this event will check for correct energy and setup a check for digestion
   // event for the _pMover object.
   (void)schedule<Test3,double>(0.1,
                          self,
                          &Test3::_afterEncounter,
                              (this == _pMover ? expected_movement_cost : 0.0));
   
}


void Test3::_afterEncounter(double movement_cost)
{
   typedef void (T3Results::*EnergyLogFunc)(bool,bool);
   EnergyLogFunc logFunc = NULL;

   if (this == _pMover)
   {
      logFunc = &T3Results::moverEnergy;
      if (expectStatueDie()) { _tr.logResult("statue life form died", _bStatueDead); }
      else { _tr.logResult("statue life form lived", ! _bStatueDead); }
   }
   else
   {
      logFunc = &T3Results::statueEnergy;
   }

   double expected = _startEnergy - (encounter_penalty + movement_cost);
   if ( energy < _startEnergy - movement_cost )
   {
      if (_close_counts(energy, expected))
      {
         (_tr.*logFunc)(true, true);
#if TESTDEBUG
         cerr << "        " << (this == _pMover ? "mover " : "statue ")
              << "energy after encounter penalty is "
              << energy << ".  That's good." << endl;
#endif
      }
      else
      {
         (_tr.*logFunc)(true, false);
#if TESTDEBUG
         cerr << " DRAT:  penalty assessed for encounter, but my\n"
                 "         energy is " << energy << ",  and it should be "
              << expected << endl;
#endif
      }
   }
   else
   {
      (_tr.*logFunc)(false, false);
#if TESTDEBUG
      cerr << " DRAT:  no penalty assessed for encounter" << endl;
#endif
   }

   // Check that the food was digested properly - both mover and statue check,
   // but only the mover should expect an increase.
   expected = energy + (_startEnergy - encounter_penalty) * eat_efficiency;
   schedule<Test3,double,double>(digestion_time + 1.0,
                                 self,
                                 &Test3::_afterDigestion,
                                    energy,
                                    (this == _pMover && expectToDigest()) ? expected : energy);
}



/**
 * Check for proper digestion - the event
 * @param   energyBefore  energy after encounter, before digestion
 * @param   energyAfter  expected energy after digestion
 */
void Test3::_afterDigestion(double energyBefore, double energyAfter)
{
#if TESTDEBUG
   cerr << "        energy was " << energyBefore << ".  After digestion time, expect near "
      << energyAfter << endl;
#endif
   if (this == _pMover && expectToDigest())
   {
      if ( energy <= energyBefore || _close_counts(energy, energyBefore) )
      {
         _tr.logResult("mover digested statue", false);
#if TESTDEBUG
   cerr << " DRAT:  But instead, I didn't digest anything" << endl;
#endif
      }
      else
      {
         _tr.logResult("mover digested statue", true);
         if (_close_counts(energy, energyAfter))
         {
            _tr.logResult("energy gained from meal is correct", true);
#if TESTDEBUG
            cerr << "        Yup, that's what I got." << endl;
#endif
         }
         else
         {
            _tr.logResult("energy gained from meal is correct", false);
#if TESTDEBUG
            cerr << " DRAT:  Well, I gained energy, but I have energy = "
                  << energy << endl;
#endif
         }
      }
   }
   else
   {
      string szLog("statue digested nothing");
      if (this == _pMover) szLog = "mover digested nothing";

      if (_close_counts(energy, energyBefore))
      {  _tr.logResult(szLog, true);
#if TESTDEBUG
            cerr << "        Yup, that's what I got." << endl;
#endif
      }
      else
      {  _tr.logResult(szLog, false);
#if TESTDEBUG
            cerr << "        However, I appear to have digested something: energy = "
               << energy << endl;
#endif
      }
   }

   if (this == _pMover)
   {
      schedule<TestResults>(1.0, _tr, &TestResults::endTest);
   }
}


/*------------------------------------------------------------------------------*/
// Test 4

/*** Static data */

RkInitializer<Test4> __Test4_initializer("Test4");


/*** From base class */

LifeForm* Test4::create(void)
{
   Test4* t4 = new Test4();
   placeObject(t4);

   return t4;
}



/*------------------------------------------------------------------------------*/
// Test 5

/*** Static data */

RkInitializer<Test5> __Test5_initializer("Test5");


/*** From base class */

LifeForm* Test5::create(void)
{
   eat_success = 0.0;      // prevent eat from being successful

   Test5* t5 = new Test5();
   placeObject(t5);

   return t5;
}



/*------------------------------------------------------------------------------*/
// Test 6

/*** Static data */

RkInitializer<Test6> __Test6_initializer("Test6");


/*** From base class */

LifeForm* Test6::create(void)
{
   eat_success = 1.0;      // allow eat, but there should be nothing to eat
                           // after the collision.

   Test6* t6 = new Test6();
   placeObject(t6);

   return t6;
}


void Test6::_lifeform_start(void)
{
   if (this == _pStatue)
   {
      _printTestTitle();
      energy = min_energy + encounter_penalty - 1.0;        // dies during encounter
#if TESTDEBUG
      cerr << "        Statue starting with energy " << energy << endl;
#endif
   }
   else
   {
      Test3::_lifeform_start();
   }
}


Test6::~Test6()
{
   if (this == _pStatue)
   {
#if TESTDEBUG
      cerr << "        Statue life form died at " << Event::now() << endl;
#endif
      // _tr.logResult("encounter with Statue life form (might be reported twice)", true);
                                          // success, otherwise, the test would have
                                          // ended before this destructor was called

            // I have to report the encounter again here.  Since the statue life form
            // dies from the encounter, it's possible that the student's
            // LifeForm::resolve_ecounter() won't call _pMover->encounter() at all.
            // I don't think that we specified in class or on the assignment that
            // they have to call encounter() on the surviving LifeForm in such
            // a case.

      // _tr.logResult("statue life form died from encounter", true);
   }
}


/*------------------------------------------------------------------------------*/
// Test 7


RkInitializer<Test7> __Test7_initializer("Test7");
T7Results Test7::_tr;
Test7* Test7::_lfA = NULL;
Test7* Test7::_lfB = NULL;


/*** From base class */

LifeForm* Test7::create(void)
{
   Test7* t7 = new Test7;

   // initialize position
   if (_tr.firstTime())
   {        // life form A
      _lfA = t7;
      t7->pos.xpos = (grid_max / 2.0) - 4.0;
      t7->pos.ypos = (grid_max / 3.0) - 3.0;
   }
   else
   {        // life form B
      if (_lfB == NULL)
      {
         _lfB = t7;
         t7->pos.xpos = (grid_max / 2.0) + 3.999;
         t7->pos.ypos = (grid_max / 3.0) - 4.0;
      }
      else
      {
         t7->pos.xpos = grid_max - 1.0;
         t7->pos.ypos = grid_max - 1.0;
      }
   }
   return t7;
}


void Test7::_lifeform_start(void)
{
   double threeFourFiveAngle = atan(3.0/4.0);
   if (this == _lfA)
   {
      cout << "two running encounter:\n";
      cout.flush();

      set_speed(5.0);
      set_course(threeFourFiveAngle);
      _missedEncounterEvent = schedule<TestResults>(2.0, _tr, &TestResults::endTest);
                                                            // a little past the collision point
   }
   else if (this == _lfB)
   {
      set_speed(5.0);
      set_course(threeFourFiveAngle + (M_PI / 2.0));
   }
}



/*** Test7-specific */

Action Test7::encounter(const ObjInfo& obj_info)
{
   if (_bFirstEncounter)
   {
      /***
       * I need to make the two life forms run apart.  Since (as per Craig's
       * instruction) most students don't actually perform the body of
       * update_position() unless significant time has passed since the last
       * one, and since one of these two life forms is likely to be _very_
       * close to the border toward which it will run now, I need to allow a
       * little bit of time to pass before running for that border.
       *
       * So here's what I do.  Call set_speed(0.0), first.  Then, go ahead
       * and call set_course() for each life form right away.  But with a
       * speed of 0.0, it won't actually start running toward the border yet.
       * Then, I set an event for 0.1 time units in the future to set my speed
       * to 5.0.  That way update_position() should go ahead and let the life
       * form move over the border as I need it to.
       */
      set_speed(0.0);

#if TESTDEBUG
      cerr << "        " << this << ": first encounter (distance=" << obj_info.distance
         << "), moving apart" << endl;
#endif
      _bFirstEncounter = false;
      set_course(obj_info.bearing + M_PI);   // move apart

      if (_missedEncounterEvent != NULL)
      {
         // life form A
         unschedule(_missedEncounterEvent);
         schedule<Test7>(0.599999, self, &Test7::goBack);
      }
      else
      {  // life form B
         schedule<LifeForm,bool>(0.6, self, &Test7::die, true);
      }
      schedule<LifeForm,double>(0.1, self, &LifeForm::set_speed, 5.0);
   }
#if TESTDEBUG
   else
   {
      cerr << "        " << this << ": subsequent encounter (distance="
         << obj_info.distance << ")" << endl;
      if (_bOnWayBack)
      {
         cerr << " DRAT:  encounter with a dead guy?" << endl;
      }
   }
#endif

   if (_bOnWayBack)
   {
      _tr.deadEncounter();
   }
   else
   {
      if (this == _lfA) _tr.logAEncounter();
      else _tr.logBEncounter();
   }

   return LIFEFORM_IGNORE;
}


void Test7::goBack()
{
   assert(this == _lfA);

#if TESTDEBUG
   cerr << "        " << this << ": turning around to trample dead one" << endl;
#endif

   set_course( position().bearing(_lfB->position()) );
   schedule<TestResults>(2.0, _tr, &TestResults::endTest);     // should only take 1.0 time
                                                               // units to reach _lfB

   _bOnWayBack = true;
}


/*------------------------------------------------------------------------------*/
// Test 8


/*** Static data */

RkInitializer<Test8> __Test8_initializer("Test8");
TestResults Test8::_tr;


/*** From base class */

LifeForm* Test8::create(void)
{
   Test8* t8 = new Test8;

   // initialize position
   if (_tr.firstTime())
   {
      t8->_me = 0;
      t8->pos.xpos = 2.0;    // top left
      t8->pos.ypos = grid_max / 3.0;
   }
   else
   {
      t8->_me = 1;
      t8->pos.xpos = 100.0;   // top right
      t8->pos.ypos = grid_max / 3.0;
   }

   return t8;
}


void Test8::_lifeform_start(void)
{
   energy = 1000.0;
   if (_me == 0)
   {
      cout << "movement cost:\n";
      cout.flush();
#if TESTDEBUG
      cerr << "        Starting energy for both Test8 life forms: " << energy << endl;
#endif

      set_course(0.0);
      set_speed(8.0);
      _runDistance = (100.0 - 2.0) + 0.1;       // Make sure I get over the border
      double delta_time = _runDistance / 8.0;

      schedule(delta_time, self, &Test8::stop_running, pos.xpos + _runDistance);
      _expected_energy = energy - movement_cost(8.0, delta_time);
#if TESTDEBUG
      cerr << "        starting movement at xpos = " << position().xpos
         << ", energy = " << energy << "\n"
         << "           expect to end up at xpos = "
         << position().xpos + _runDistance << ", energy = "
         << _expected_energy << endl;
#endif
   }
   else
   {
      assert(_me == 1);
      set_speed(0.0);
   }
}


void Test8::stop_running(double expected_xpos)
{
   if (! _close_counts(pos.xpos, expected_xpos, 0.2) )
   {
      _tr.logResult("position after running looks good", false);
#if TESTDEBUG
      cerr << " DRAT:  Hmm, I should be at x-coordinate " << expected_xpos
            << ", but instead I'm at " << pos.xpos << endl;
#endif
   }
   else _tr.logResult("position after running looks good", true);

   set_speed(0.0);
   schedule(0.0, self, &Test8::check_health_and_die);
}


void Test8::check_health_and_die()
{
   _tr.logResult("some movement penalty assessed", energy < 1000.0 - 0.001);

   if (  (energy >= _expected_energy  &&  (energy - _expected_energy) < VERY_CLOSE * 2.0)
         || (energy < _expected_energy  &&  (_expected_energy - energy) < VERY_CLOSE * 2.0)
      )
   {
      _tr.logResult("movement energy penalty correct", true);
#if TESTDEBUG
      cerr << "        After running, my energy is " << energy << ", as expected" << endl;
#endif
   }
   else
   {
      _tr.logResult("movement energy penalty correct", false);
#if TESTDEBUG
      cerr << " DRAT:  After running, my energy is " << energy
            << ", but I expected " << _expected_energy << endl;
#endif
   }

   schedule<TestResults>(0.0, _tr, &TestResults::endTest);
}


/*------------------------------------------------------------------------------*/
// Test 9


/*** Static data */

RkInitializer<Test9> __Test9_initializer("Test9");
TestResults Test9::_tr;


/*** From base class */

LifeForm* Test9::create(void)
{
   Test9* t9 = new Test9;

   // initialize position
   t9->pos.xpos = grid_max - 25.0;
   t9->pos.ypos = grid_max - 50.0;

   return t9;
}


void Test9::_lifeform_start(void)
{
   energy = 1000.0;

   cout << "Christopher Columbus in a parallel universe:\n";
   cout.flush();

#if TESTDEBUG
      cerr << "        Starting energy for Test9 life form: " << energy << endl;
#endif

   set_course(M_PI / 4.0);    // 45 degrees
   set_speed(8.0);
   double distToEnd = 1.4142135623731 * 25.0;      // course ((x/y) len) * sqrt(2)
   double runDistance = distToEnd + 1.0;           // plus 1.0 nudge past edge

   double delta_time = runDistance / 8.0;

   schedule<TestResults,string,bool>(  delta_time,
                                       _tr,
                                       &TestResults::failedToDie,
                                          "Christopher",
                                          true
                                    );

   _expectedDeathTime = Event::now() + (distToEnd / 8.0);
}


Test9::~Test9()
{
   if ( _close_counts(Event::now(), _expectedDeathTime,
                     (0.1 /* lee-way dist */) / 8.0 /* speed */)
      )
   {
      _tr.logResult("Christopher died when expected", true);
#if TESTDEBUG
      cerr << "        Christopher died at " << Event::now() << " as expected" << endl;
#endif
   }
   else
   {
      _tr.failedToDie("Christopher", false);
#if TESTDEBUG
      cerr << " DRAT:  Christopher died, but at time " << Event::now()
            << ".\n         I was expecting something near time " << _expectedDeathTime
            << endl;
#endif
   }
   schedule<TestResults>(0.0, _tr, &TestResults::endTest);
}


/*------------------------------------------------------------------------------*/
// Test 10

/*** Static data */

RkInitializer<Test10> __Test10_initializer("Test10");
T10Results Test10::_tr;


/*** From base class */

LifeForm* Test10::create(void)
{
   g_use_algae_spores = true;

   Test10* t10 = new Test10();
   t10->pos.xpos = grid_max - 0.5;
   t10->pos.ypos = grid_max - 0.5;

   return t10;
}


void Test10::_lifeform_start(void)
{
   cout << "event queue monitor:\n";
   cout.flush();

   schedule<T10Results>(1.0, _tr, &T10Results::monitorQueue);
}


/*------------------------------------------------------------------------------*/
// Test 11

/*** Static data */

RkInitializer<Test11> __Test11_initializer("Test11");


/*** From base class */

LifeForm* Test11::create(void)
{
   g_use_algae_spores = true;

   Test11* t11 = new Test11();
   t11->pos.xpos = grid_max - 0.5;
   t11->pos.ypos = grid_max - 0.5;

   return t11;
}
