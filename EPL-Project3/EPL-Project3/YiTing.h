#ifndef _YiTing_h
#define _YiTIng_h

#include "LifeForm.h"
#include "Init.h"

struct myComp
{
	bool operator() (const ObjInfo& lhs, const ObjInfo& rhs)
	{
		return lhs.distance < rhs.distance;
	}
};


class YiTing : public LifeForm {

protected:
	std::string myName;
	double nameUpdateTime;
	double myTailSpeed;
	double my_reproduce_time;
	Event* huntEvent;

	void spawn(void);
	static void initialize(void);
	bool checkSpawn(void);
	void strategyFirst(double, double, double, double, double, double, const ObjInfo&);
	bool checkEnemy(double);
	void hunt(void);
	void updateName(const ObjList&);
	bool huntPrey(const ObjList&);
	bool canEat(const ObjInfo&);
	
	
	


public:
	friend class Initializer<YiTing>;

	std::string player_name(void) const;
	std::string species_name(void) const;
	Color my_color(void) const;   // defines LifeForm::my_color
	YiTing(void);
	~YiTing(void);
	static LifeForm* create(void);
	Action encounter(const ObjInfo&);
	

	//unfinished
	
};

#endif  /* _YiTing_h */