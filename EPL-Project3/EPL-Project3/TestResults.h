#include <iostream>
#include <deque>
#include <utility>
#include <strstream>
#include <string>
using namespace std;


#if !defined(__TESTRESULTS_H__)
#define __TESTRESULTS_H__


class TestResults
{
public:
   TestResults(): _bAllPassed(true), _max_str_len(0) {;}
   virtual ~TestResults() {;}


   bool firstTime()
   {  static bool bFirstTime = true;
      bool rc = bFirstTime;
      bFirstTime = false;
      return rc;
   }


   /*** Score keeping */
   void logResult(string str, bool success)
   {
      if (!success) _bAllPassed = false;

      if (_max_str_len < str.length())
      {  _max_str_len = str.length();
      }
      _results.push_back(pair<string,bool>(str,success));
   }

   bool allLogsPassed() const { return _bAllPassed; }

   void printResults();


   /*** Failing to die */
   void failedToDie(string szWho, bool bEndTest)
   {
      logResult(szWho + " died when expected", false);
      if (bEndTest)
      {  finalize();
      }
   }


   // to create an event to finish, if you need it
   void endTest() { finalize(); }


   // Virtual function called by endTest().  Over-ride for test-specific
   // exit code.
   virtual void finalize();

private:
   typedef pair<string,bool> ResPair;
   deque<ResPair> _results;
   bool _bAllPassed;
   int _max_str_len;

   static string _defaultPropertyStr;
};


/*-------------------------------------------------------------------------------*/


/**
 * Test1 life form Should perform two perceives.
 * Each should return 4 objects.
 */
class T1Results : public TestResults
{
public:
   T1Results()
      :  _nExpected(2), _nActual(0), _bPerceiveWorked(true), _bBearingsDistOk(true),
         _bPenaltyAssessed(true), _bPenaltyOk(true), _bDead(false) {;}
   virtual ~T1Results() {;}

   int expectedPerceives() { return _nExpected; }
   void incrementPerceives() { _nActual++; }

   void goodPerceive() {;}
   void badPerceive() { _bPerceiveWorked = false; }

   void goodBearingsDist() {;}
   void badBearingsDist() { _bBearingsDistOk = false; }

   void penaltyAssessed(bool bAssessed)
      { _bPenaltyAssessed = (bAssessed ? _bPenaltyAssessed : false); }

   void penaltyOk(bool bOk)
      { _bPenaltyOk = (bOk ? _bPenaltyOk : false); }

   void iDied() { _bDead = true; }

   virtual void finalize();

private:
   int _nExpected;
   int _nActual;
   bool _bPerceiveWorked;
   bool _bBearingsDistOk;
   bool _bPenaltyAssessed;
   bool _bPenaltyOk;
   bool _bDead;
};


/*-------------------------------------------------------------------------------*/


/**
 * Test2 tests reproduction and then aging.
 */
class T2Results : public TestResults
{
public:
   T2Results():
      _startEnergy(0.0),
      _childrenCreated(0)
      {;}

   virtual ~T2Results() {;}

   void setStartEnergy(double startEnergy);
   double startEnergy() { return _startEnergy; }

   void childCreated();

   void childEnergy(bool bSomePenalty, bool bGoodPenalty)
   {  logResult("reproduction - some child energy penalty assessed", bSomePenalty);
      logResult("reproduction - child energy penalty correct", bGoodPenalty);
   }

   void parentEnergy(bool bSomePenalty, bool bGoodPenalty)
   {  logResult("reproduction - some parent energy penalty assessed", bSomePenalty);
      logResult("reproduction - parent energy penalty correct", bGoodPenalty);
   }

   void childDistanceGood(bool bGoodDist)
   {  logResult("reproduction - child distance from parent", bGoodDist);
   }

   void childAgingOk(bool bSomePenalty, bool bGoodAgePenalty)
   {  logResult("aging - some penalty assessed", bSomePenalty);
      logResult("aging - correct penalty assessed", bGoodAgePenalty);
   }

   virtual void finalize();

private:
   double _startEnergy;
   int _childrenCreated;
};


/*-------------------------------------------------------------------------------*/


/**
 * Tests 3, 4, and 5 test what happens when two life forms have
 * an encounter.  In test 3, one tries to eat the other, and it should
 * succeed, b/c I've rigged eat_success_chance().  In test 4, they ignore
 * each other, so I test to see that neitehr dies or digest any new energy.
 * Finally, in test 5, one tries to eat the other, but I've rigged
 * eat_success_chance() to return 0.0, so I expect nothing to die or gain
 * energy again.
 */
class T3Results : public TestResults
{
public:
   T3Results() {;}
   virtual ~T3Results() {;}

   void reachedOrPassedStatue(bool bSuccess, bool bEndTest);

   void moverEnergy(bool bSomePenalty, bool bGoodPenalty)
   {  logResult("moving life form - some energy penalty assessed", bSomePenalty);
      logResult("moving life form - energy penalty correct", bGoodPenalty);
   }

   void statueEnergy(bool bSomePenalty, bool bGoodPenalty)
   {  logResult("statue life form - some energy penalty assessed", bSomePenalty);
      logResult("statue life form - energy penalty correct", bGoodPenalty);
   }
};


/*-------------------------------------------------------------------------------*/


/**
 * Test 7 tests for an encounter when two life forms are both
 * Two life forms move together, encounter, and then move apart.
 * One them commits suicide and the other moves over the dead life
 * form (no encounter should occur).
 */
class T7Results : public TestResults
{
public:
   T7Results(): _nAEncounterNotices(0), _nBEncounterNotices(0), _bDeadEncounter(false) {;}
   virtual ~T7Results() {;}

   void logAEncounter() { ++_nAEncounterNotices; }
   void logBEncounter() { ++_nBEncounterNotices; }

   void deadEncounter() { _bDeadEncounter = true; }

   // Virtual function called by endTest().  Over-ride for test-specific
   // exit code.
   virtual void finalize()
   {
      logResult("encounter occurred", _nAEncounterNotices > 0 && _nBEncounterNotices > 0);
      logResult("1 or 2 encounters each",
                  (_nAEncounterNotices == 1 || _nAEncounterNotices == 2)
                  && (_nBEncounterNotices == 1 || _nBEncounterNotices == 2)
                  && (_nAEncounterNotices == _nBEncounterNotices)
               );
      logResult("no encounter with dead life form", !_bDeadEncounter);
#if TESTDEBUG
      cerr << "        life form A had " << _nAEncounterNotices << " encounters" << endl;
      cerr << "        life form B had " << _nBEncounterNotices << " encounters" << endl;
#endif

      TestResults::finalize();
   }

private:
   int _nAEncounterNotices;   // should be 1 or 2 - one when the life forms
   int _nBEncounterNotices;   // move together, and possibly another when they
                              // move apart.
   bool _bDeadEncounter;

};


/*-------------------------------------------------------------------------------*/


/**
 * Test 10.
 *
 * The T10Results class runs sanity checks on the event queue every so often
 * to see if everything looks ok.  I just let a bunch of Craigs, Rocks, Algae,
 * Squares, Baits, etc. run around and do their thing, and every once in a while
 * check things like:
 *
 *    -> there's exactly 1 border_cross event for every life form whose speed
 *       is non-zero
 *    -> there's no border_cross event for every life form whose speed is 0
 *    -> no events for life forms with energy < min_energy, unless there is
 *       a pending _die() event
 *    -> all life forms with events in the queue are alive (i.e., is_alive == true)
 */
class T10Results : public TestResults
{
public:
   T10Results():
      _bStrayEvents(false),
      _bDeadProblem(false),
      _bLowEnergyProblem(false),
      _bMissingBorderCross(false),
      _bUnwantedBorderCross(false),
      _bExtraBorderCross(false)
      {;}
   virtual ~T10Results() {;}

   void monitorQueue();

   virtual void finalize();

private:
   bool _bStrayEvents;
   bool _bDeadProblem;
   bool _bLowEnergyProblem;
   bool _bMissingBorderCross;
   bool _bUnwantedBorderCross;
   bool _bExtraBorderCross;

   static double _period;
};


#endif
