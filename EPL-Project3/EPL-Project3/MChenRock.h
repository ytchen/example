#if !(_MChenRock_h)
#define _MChenRock_h 1

#include "LifeForm.h"
#include "Init.h"

class MChenRock : public LifeForm {
protected:
	int Tsplit;
	int course_changed ;
	static void initialize(void);
	void spawn(void);
	void hunt(void);
	void HuntEncounter(void);
	void live(void);
	void ST(bool);
	void SetSpeed();
	Event* hunt_event;
	Event* hunt_event_encounter;
	ObjInfo HitObj;
	int countADhunt;
	int countAD;
	double dTimeSS,dTimeFre;
	bool bAttack;
	double myspeed,mycourse;
	double reproduceC;
	double evalution;
	int attackercount;
	bool bRock;
public:
	MChenRock(void);
	~MChenRock(void);
	Color my_color(void) const;   // defines LifeForm::my_color
	static LifeForm* create(void);
	std::string species_name(void) const;
	std::string player_name(void) const;
	Action encounter(const ObjInfo&);
	friend class Initializer<MChenRock>;
};


#endif /* !(_MChenRock_h) */
