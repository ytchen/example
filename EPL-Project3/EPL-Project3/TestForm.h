#if !(_TestForm_h)
#define _TestForm_h 1
#include <string>
#include "Event.h"
#include "LifeForm.h"
#include "Schedule.h"
#include "TestResults.h"
#include "RkInit.h"

using namespace std;

/*--------------------------------------------------------------------*/

#if !defined(VERY_SMALL)
#define VERY_SMALL 1.0e-4
#endif

#if !defined(VERY_CLOSE)
/**
 * double variables should be this close to be considered as the same value.
 */
#define VERY_CLOSE VERY_SMALL
#endif

/**
 * Check if the two arguments are close to the same value.<br>
 * @param   a  first double arg
 * @param   b  second double arg
 */
bool _close_counts(double a, double b, double fudge = VERY_CLOSE);


/*--------------------------------------------------------------------*/

#if !defined(self)
#define self (*this)
#endif

/**
 * LifeForm-derived class for testing student LifeForm submissions.<br>
 * This class is a friend class to LifeForm, so it can access private
 * data members and verify expected behavior of the simulator.
 */
class TestForm : public LifeForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void) { return NULL; }

   /**
    * Declaration for the RkInitializer object that enters TestForm into the species table.
    */
   friend class RkInitializer<TestForm>;

//@}

protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void) = 0;

//@}


public:

   /********************************************************
    * REMEMBER: do not call set_course, set_speed, perceive,
    * or reproduce from inside the constructor!!!!
    * Must wait until the object is actually alive
    */
   TestForm(string name = string("TestForm"))
      : _testform_name(name) { schedule(VERY_SMALL, self, &TestForm::_begin); }


   /// Destructor
   virtual ~TestForm(void) {;}


private:
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   void _begin(void) { _lifeform_start(); }


public:
   /**
    * Return color for drawing my life form.
    * @return  The color to use.
    */
   virtual Color my_color(void) const { return CYAN; }  

   /**
    * For identification in perceive(), and status reporting.
    * @return  an identification string.
    */
   virtual string species_name(void) const { return _testform_name; }

   /**
    * Choose to LIFEFORM_EAT or LIFEFORM_IGNORE another LifeForm during an encounter.
    * @param   obj_info  information regarding the other LifeForm.
    */
   virtual Action encounter(const ObjInfo& obj_info) { return LIFEFORM_IGNORE; }

   /**
    * Good for creating an event to check your energy level in the
    * future.
    * @param   expected_value  what energy should be
    * @param   health_ok  boolean variable to set to false
    *          if energy is not equal to expected_value
    */
   void check_health(double expected_value, string check_for, bool* health_ok);


protected:

   /**
    * Force rads between 0 and 2*M_PI
    */
   double _normalize_angle(double rads) const;



private:

   /**
    * Holds identification string for this TestForm life form.
    */
   string _testform_name;

   ///
   static bool _lived_too_long;
};




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


/**
 * Test 1.
 * Call perceive() over and over, checking for proper penalty
 * and that the Test1 life form dies when it's supposed to.
 * Check the bearing returned for each object in the list.
 */
class Test1 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test1>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test1() {;}

   /// Destructor
   virtual ~Test1();


private:
   static T1Results _tr;

   void _doPerceive();
   void _checkEnergy(double prev_energy, double expected);
};



/*-------------------------------------------------------------*/

/**
 * Test 2.
 * Call reproduce() again and again, checking energy of both the
 * parent and the child.  The parent should die while trying to
 * reproduce() a second time, so only one child should successfully
 * be created.  Then, I watch the child age to death, to make sure
 * aging works right.
 */
class Test2 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test2>;
//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test2(): _bEverLived(false), _bCheckAging(false), _pFailedToDie(NULL)
      { age_frequency = 100.0; }

   /// Destructor
   virtual ~Test2();


private:
   /// the TestResults object for test 2
   static T2Results _tr;

   /// first Test2 life form - creted by LifeForm::create_life()
   static Test2* _pParent;

   static bool _bDidChildTestsAlready;

   bool _bEverLived;    // 2nd child will be created and then immediately deleted

   bool _bCheckAging;   // set to true only for first (and hopefully only) child created

   /// Keep pointer to failed to die event - if I die, delete it
   Event* _pFailedToDie;

   /// If I die before this time, then delete _pFailedToDie
   double _failedToDieTime;



   /// Reproduce myself
   void _doReproduce();

   /// Child checks whether it is within reproduce_dist units from the parent.
   void _checkDist();

   // checks if this life form lived beyond when was expected
   void _failedToDie(string szWho)
   {
      _tr.logResult(szWho + " died when expected", false);
   }



   void _checkAgingEnergy(double prev_energy, double expected);
};




/*-------------------------------------------------------------*/

/**
 * Test 3.
 * First life form moves toward the second, and tries to eat it.  Check for encounter
 * penalty, "try to eat" penalty, and correct added energy upon digestion.
 */
class Test3 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   static void placeObject(Test3* pLife);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test3>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

   virtual void _printTestTitle() { cout << "encounter - eat other:" << endl; }

//@}


public:

   /// Constructor
   Test3() : _pPassStatueEvent(NULL), _pFinishAfterPassed(NULL), _startMoveTime(-1.0) {;}

   /// Destructor
   virtual ~Test3() { if (this == _pStatue) _bStatueDead = true; }

   /**
    * Choose to LIFEFORM_EAT or LIFEFORM_IGNORE another LifeForm during an encounter.
    * @param   obj_info  information regarding the other LifeForm.
    */
   virtual Action encounter(const ObjInfo& obj_info);

   virtual Action encounterAction() const { return LIFEFORM_EAT; }

   virtual bool expectToDigest() const
      { return (encounterAction() == LIFEFORM_EAT && _close_counts(eat_success, 1.0)); }

   virtual bool expectStatueDie() const { return expectToDigest(); }

   virtual bool missedEncounterHalt() const { return true; }

   /**
    * Check for encounter penalty and setup event to check for digestion energy change
    * @param   movement_cost  how much energy did this life form lose from
    *                         moving before the encounter?
    */
   void _afterEncounter(double movement_cost);


   /**
    * In one test so far, the test needs to continue if the Test3-derived
    * life form does not receive an encounter event from the simulator.  This
    * is because the other life form dies from the collision, and we haven't
    * defined whether encounter() should be called on the other life form
    * or not.  So it's legal to not get an encounter.
    */
   void _finishAfterNoEncounter();


   /**
    * Check for proper digestion
    * @param   energyBefore  energy after encounter, before digestion
    * @param   energyAfter  expected energy after digestion
    */
   void _afterDigestion(double energyBefore, double energyAfter);

private:
   Event* _pPassStatueEvent;     // these three only used by _pMover life form
   Event* _pFinishAfterPassed;
   double _startMoveTime;

   static double _startEnergy;
   static bool _bStatueDead;


protected:
   static T3Results _tr;

   static Test3* _pMover;
   static Test3* _pStatue;

};





/*-------------------------------------------------------------*/


/**
 * Test 4.
 * First life form moves toward the second, and both ignore each other.
 * Check for encounter penalty and ensure that both life forms live and
 * neither gains energy.
 */
class Test4 : public Test3
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test4>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{

   virtual void _printTestTitle() { cout << "encounter - both ignore:" << endl; }

//@}


public:

   /// Constructor
   Test4() {;}

   /// Destructor
   virtual ~Test4() {;}

   /**
    * Choose to LIFEFORM_IGNORE.
    */
   virtual Action encounterAction() const { return LIFEFORM_IGNORE; }
};




/*-------------------------------------------------------------*/


/**
 * Test 5.
 * First life form moves toward the second, and tries to eat it, but I've
 * set eat_success = 0.0, so it should fail.
 * Check for encounter penalty and ensure that both life forms live and
 * neither gains energy.
 */
class Test5 : public Test3
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test5>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{

   virtual void _printTestTitle() { cout << "encounter - eat other but can't:" << endl; }

//@}


public:

   /// Constructor
   Test5() {;}

   /// Destructor
   virtual ~Test5() {;}

   /**
    * Choose to LIFEFORM_EAT.
    */
   virtual Action encounterAction() { return LIFEFORM_EAT; }
};




/*-------------------------------------------------------------*/


/**
 * Test 6.
 * First life form moves toward the second, and tries to eat it.  eat_success
 * is set to 1.0, but the life form it's going to try to eat starts with too
 * little energy, so it dies from the collision.  Check to see that the non-
 * moving life form (i.e., the statue) dies from the collision, and make sure
 * the eating life form does not gain any energy.
 */
class Test6 : public Test3
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test6>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{

   virtual void _printTestTitle() { cout << "encounter - eat other but it's dead:" << endl; }

   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test6() {;}

   /// Destructor
   virtual ~Test6();

   virtual bool expectToDigest() const { return false; }
   virtual bool expectStatueDie() const { return true; }
   virtual bool missedEncounterHalt() const { return false; }
};




/*-------------------------------------------------------------*/

/**
 * Test 7.
 * Strategically place two Test7 life forms, and have them move toward
 * one another.  Check to see that they encountered.
 *
 *        4   |   3
 *    --------E E-----     Two life forms start at A and B, and encounter when
 *    |      /|  \   |     they're at E.
 *  3 |   /   |   \  | 4
 *    |/  5      5 \ |     They then move apart, A commits suicide, and B
 *    A             \|     turns around and tramples over B's dead body (to
 *                   B     ensure there's no encounter after A is dead).
 *
 * In this test I just check to make sure an encounter occurs, and also
 * that no encounter occurs after A commits suicide.  Also, because of the
 * placement of A and B when they encounter, there should be at most two
 * encounters each as they move together and then apart.
 */
class Test7 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test7>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test7(): _missedEncounterEvent(NULL), _bFirstEncounter(true),_bOnWayBack(false) {;}

   /// Destructor
   virtual ~Test7() {;}

   /// Once we we see the encounter, go ahead and die
   virtual Action encounter(const ObjInfo& obj_info);

   /// life form A turns around and tramples dead life form B
   void goBack();


private:
   Event* _missedEncounterEvent;
   bool _bFirstEncounter;

   bool _bOnWayBack;          // after two life forms have successfully moved
                              // apart and A starts back, they should not encounter

   static T7Results _tr;
   static Test7* _lfA;
   static Test7* _lfB;
};



/*-------------------------------------------------------------*/

/**
 * Test 8.
 *
 * Check that I can move correctly and get charged the proper penalty for
 * movement.  I place two life forms strategically, and have one of them barely
 * enter the other's quadrant, then stop, then check to see that I'm where I
 * think I should be, and that my energy is right.
 */
class Test8 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test8>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test8() {;}

   /// Destructor
   virtual ~Test8() {;}

   /// Called after I don't need to run anymore.
   void stop_running(double expected_xpos);

   /// Commit suicide.
   void check_health_and_die();

private:
   int _me;
   double _expected_energy;
   double _runDistance;

   static TestResults _tr;
};


/*-------------------------------------------------------------*/

/**
 * Test 9.
 *
 * Run off the edge of the world.  Make sure I die.
 */
class Test9 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test9>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test9() {;}

   /// Destructor
   virtual ~Test9();

private:
   double _expectedDeathTime;

   static TestResults _tr;
};


/*-------------------------------------------------------------*/

/**
 * Test 10.
 *
 * The T10Results class runs sanity checks on the event queue every so often
 * to see if everything looks ok.  The Test10 life form simply starts the
 * T10Results object and then dies.  The T10Results object needs to be started
 * after the world and Event queue have been created and are ready to accept
 * events.
 *
 * See the comment header for class T10Results in TestResults.h for details
 * about test 10.
 */
class Test10 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test10>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void);

//@}


public:

   /// Constructor
   Test10() {;}

   /// Destructor
   virtual ~Test10() {;}

private:
   static T10Results _tr;
};



/*-------------------------------------------------------------*/

/**
 * Test 11.
 *
 * In Test 11 I just run the simulator with several life forms, watch it run
 * and make a judgement about how well it runs.  Does it halt from time to
 * time, does it seg fault, etc.
 *
 * I need the Test11 life form only to set g_use_algae_spores to true.
 */
class Test11 : public TestForm
{
public:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * Used by LifeForm to create a new TestForm.
    * @return  the new TestForm life form.
    */
   static LifeForm* create(void);

   /**
    * Declaration for the RkInitializer object that enters Test1 into the species table.
    */
   friend class RkInitializer<Test11>;


//@}


protected:

/** @name Required Derived-class methods and data!!!!! */
//@{
   /**
    * The TestForm starts the test in this method.<br>
    * Derived classes override this method to perform a different test.
    */
   virtual void _lifeform_start(void) {;}

//@}


public:

   /// Constructor
   Test11() {;}

   /// Destructor
   virtual ~Test11() {;}
};



#endif /* !(_TestForm_h) */
