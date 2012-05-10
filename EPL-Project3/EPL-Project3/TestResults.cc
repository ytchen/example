
#include <map>
#include "Params.h"
#include "Schedule.h"
#include "Event.h"
#include "QuadTree.h"
#include "LifeForm.h"
#include "TestForm.h"

#include "TestResults.h"
using namespace std;

/*-------------------------------------------------------------------------------*/

string TestResults::_defaultPropertyStr("");

void TestResults::printResults()
{
   _max_str_len += 8;

   while(! _results.empty())
   {
      ResPair pair = _results.front();
      _results.pop_front();

      pair.first = string("     ") + pair.first;
      pair.first += ":  ";

      int len = pair.first.length();
      while (len < _max_str_len)
      {
         pair.first += ' ';
         len++;
      }

      cout << pair.first << (pair.second ? "passed" : "failed") << endl;
   }

   cout << "     end test at time " << Event::now() <<  endl;
}


/**
 * Virtual function called by endTest().  Over-ride for test-specific exit
 * code.<p>
 *
 * I strongly suggest you call this base-class method within your
 * over-ride.
 */
void TestResults::finalize()
{
   LifeForm* lf = LifeForm::all_life;
   while(lf != NULL)
   {
      if (! LifeForm::space.is_out_of_bounds(lf->pos)
          && LifeForm::space.is_occupied(lf->pos))
      {
         LifeForm::space.remove(lf->pos);
      }
      Event::delete_matching(lf);
      lf->is_alive = false;

      LifeForm* next = lf->next_life;
      delete lf;
      lf = next;
   }
   LifeForm::all_life = NULL;

   printResults();
   exit( (allLogsPassed() ? 0 : 1) );
}

/*-------------------------------------------------------------------------------*/

void T1Results::finalize()
{
   logResult("perceive should return 4 life forms", _bPerceiveWorked);
   if (_bPerceiveWorked)
   {
      logResult("bearing and distance reported", _bBearingsDistOk);
      logResult("perceive penalty assessed", _bPenaltyAssessed);
      if (_bPenaltyAssessed)
      {
         logResult("penalty for correct amount", _bPenaltyOk);
      }
   }
   logResult("expected 2 perceive() calls", _nActual == _nExpected);
   logResult("test life form died ok", _bDead);


   TestResults::finalize();
}

/*-------------------------------------------------------------------------------*/

void T2Results::setStartEnergy(double startEnergy)
{
   _startEnergy = startEnergy;
#if TESTDEBUG
   cerr << "        parent Test2 life form starting with energy " << startEnergy << endl;
#endif
}

void T2Results::childCreated()
{
   ++_childrenCreated;

   if (_childrenCreated == 1)
   {
      logResult("child created", _childrenCreated > 0);
   }
}

void T2Results::finalize()
{
   logResult("created exactly 1 child", _childrenCreated == 1);
   TestResults::finalize();
}


/*-------------------------------------------------------------------------------*/

void T3Results::reachedOrPassedStatue(bool bSuccess, bool bEndTest)
{
   logResult("encounter with Statue life form", bSuccess);
   if (bEndTest)
   {           // can't test anything else really
      schedule<TestResults>(0.0, *this, &T3Results::endTest);
   }
}

/*-------------------------------------------------------------------------------*/
/*
class PtrHash : public hash<int>
{
public:
   size_t operator()(void* p) const { return hash<int>::operator()( (int)p ); }
};

*/
class PtrEqual
{
public:
   bool operator() (void* a, void* b) const { return a == b;}
};
class PtrLess
{
public:
   bool operator() (void* a, void* b) const { return a < b;}
};


/*** Static data */
double T10Results::_period = 40.0;


void T10Results::monitorQueue()
{
#if TESTDEBUG
   cerr << "        checking event queue at " << Event::now() << endl;
#endif
   vector<Event*>& events = Event::equeue.V;
   typedef vector<Event*>::iterator VIter;

   //typedef multimap<void*,Event*,PtrHash,PtrEqual> ObjEventsMap;
	 typedef multimap<void*,Event*,PtrLess> ObjEventsMap;
   ObjEventsMap eventsByObj;

   for (VIter vIter = events.begin(); vIter != events.end(); ++vIter)
   {
      eventsByObj.insert(ObjEventsMap::value_type((*vIter)->operates_on(), *vIter));
   }


   pair<ObjEventsMap::const_iterator, ObjEventsMap::const_iterator> lfEventRange;
#if 0
   if (!_bStrayEvents)
   {
      lfEventRange = eventsByObj.equal_range((void*)NULL);
      if (lfEventRange.first != lfEventRange.second)
      {
         cerr << " DRAT:  at least one event in the queue is not tied to a LifeForm object"
            << " (time=" << Event::now() << ")" << endl;
      }
      else _bStrayEvents = true;
   }
#endif

   bool bDeadProblem = false;
   bool bLowEnergyProblem = false;
   bool bUnwantedBorderCross = false;
   bool bMissingBorderCross = false;
   bool bExtraBorderCross = false;

   for (LifeForm* lf = LifeForm::all_life; lf != NULL; lf = lf->next_life)
   {
      lfEventRange = eventsByObj.equal_range(lf);


      /*** check if this life form is dead or should be, and yet still exists */
      if (! bDeadProblem
          && (lf->energy < min_energy || !lf->is_alive)
         )
      {
         bool blfDieEvent = false;
         for (ObjEventsMap::const_iterator it = lfEventRange.first;
              it != lfEventRange.second;
              ++it)
         {
            /*** is Event a die() or _die() event */
            _Procedure0* p0 = it->second->doit.core;
            _Procedure0_objbound<LifeForm>* lfProc
                  = dynamic_cast< _Procedure0_objbound<LifeForm>* >(p0);
            if (lfProc != NULL && lfProc->fun == &LifeForm::_die)
            {  blfDieEvent = true;
            }
            else
            {
               _Procedure0* p0 = it->second->doit.core;
               _Procedure1_objbound<LifeForm,bool>* lfProc
                     = dynamic_cast< _Procedure1_objbound<LifeForm,bool>* >(p0);
               if (lfProc != NULL /* && lfProc->fun == &LifeForm::die */)
               {
                                 // ^^^ have to assume it's a die() event, b/c
                                 // g++ doesn't define the operator==() I need
                                 // here.
                  blfDieEvent = true;
               }
            }
         }
         
         if (!lf->is_alive && !blfDieEvent)
         {
#if TESTDEBUG
            cerr << " DRAT:  life form " << lf << " has is_alive == false, yet exists"
               << " (time=" << Event::now() << ")" << endl;
#endif
            _bDeadProblem = bDeadProblem = true;
         }
         else if (lf->energy < min_energy && !blfDieEvent)
         {
#if TESTDEBUG
            cerr << " DRAT:  life form " << lf << " has too little energy (" << lf->energy
               << ") and is not dying (time=" << Event::now() << ")" << endl;
#endif
            _bLowEnergyProblem = bLowEnergyProblem = true;
         }
      }

      
      /*** check if this life form is stationary and yet has a border_cross() event */
      if ( !bUnwantedBorderCross  &&  _close_counts(lf->get_speed(), 0.0) )
      {
         for (ObjEventsMap::const_iterator it = lfEventRange.first;
              !bUnwantedBorderCross  &&  it != lfEventRange.second;
              ++it)
         {
            // is Event a border_cross() event?
            _Procedure0* p0 = it->second->doit.core;
            _Procedure0_objbound<LifeForm>* lfProc
                  = dynamic_cast< _Procedure0_objbound<LifeForm>* >(p0);
            if (lfProc != NULL && lfProc->fun == &LifeForm::border_cross)
            {
#if TESTDEBUG
               cerr << " DRAT:  life form " << lf << " has a pending "
                  << "border_cross() event, even though it is not moving (time="
                  << Event::now() << ")" << endl;
#endif
               _bUnwantedBorderCross = bUnwantedBorderCross = true;
            }
         }
      }


      /*** check if this life form has no border_cross() event or more than one */
      if ((!bMissingBorderCross || !bExtraBorderCross)
           && lf->get_speed() > 0.0
         )
      {
         bool blfBorderCross = false;
         for (ObjEventsMap::const_iterator it = lfEventRange.first;
              (!bMissingBorderCross || !bExtraBorderCross)  &&  it != lfEventRange.second;
              ++it)
         {
            // is Event a border_cross() event?
            _Procedure0* p0 = it->second->doit.core;
            _Procedure0_objbound<LifeForm>* lfProc
                  = dynamic_cast< _Procedure0_objbound<LifeForm>* >(p0);
            if (lfProc != NULL && lfProc->fun == &LifeForm::border_cross)
            {
               if (blfBorderCross && !bExtraBorderCross)
               {
#if TESTDEBUG
                  cerr << " DRAT:  life form " << lf << " has more than one"
                     << " border cross event (time=" << Event::now() << ")" << endl;
#endif
                  _bExtraBorderCross = bExtraBorderCross = true;
               }
               else blfBorderCross = true;
            }
         }

         /*** Make sure a moving life form has a pending border_cross() event */
         if (!bMissingBorderCross && lf->is_alive && !blfBorderCross)
         {
#if TESTDEBUG
            cerr << " DRAT:  life form " << lf << " is moving (speed="
               << lf->get_speed() << "), but has no border_cross() event"
               << " (time=" << Event::now() << ")" << endl;
#endif
            _bMissingBorderCross = bMissingBorderCross = true;
         }
      }


   } // for - over LifeForms in LifeForm::all_life list


   /*** setup next time or end the test */
   static int iterations = 0;
   ++iterations;
   if (iterations < 6)
   {
      schedule<T10Results>(T10Results::_period, self, &T10Results::monitorQueue);
   }
   else
   {
      schedule<TestResults>(0.0, self, &T10Results::endTest);
   }
#if TESTDEBUG
   cerr << "        done checking event queue" << endl;
#endif
}


void T10Results::finalize()
{
   logResult("no dead life forms running around", !_bDeadProblem);
   logResult("no life forms with energy < min_energy", !_bLowEnergyProblem);
   logResult("all moving life forms have pending border cross events", !_bMissingBorderCross);
   logResult("life forms have <= 1 border cross events", !_bExtraBorderCross);
   logResult("no border cross events for non-moving life forms", !_bUnwantedBorderCross);

   TestResults::finalize();
}
