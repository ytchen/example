#include <iostream>
#include <stdlib.h>
#include <cmath>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "String.h"
#include "ObjInfo.h"
#include "Set.h"
#include "YiTing.h"
#include "Window.h"
#include <algorithm>
#include <map>

using namespace std;

Initializer<YiTing> __YiTing_initializer;

String YiTing::species_name(void) const
{
	return myName;
}

String YiTing::player_name(void) const
{
	return "YiTing";
}

Color YiTing::my_color(void) const
{
	return MAGENTA;
}

YiTing::YiTing(void) : myName("YiTing"), nameUpdateTime(0.0), myTailSpeed(0.009527), my_reproduce_time(-10.0)
{
	set_course(drand48() * 2.0 * M_PI);
	set_speed(5 + myTailSpeed);
	huntEvent = Nil<Event>();
	huntEvent = new Event(0.1, make_procedure(*this, &YiTing::hunt));
}

YiTing::~YiTing(void)
{
}

LifeForm* YiTing::create(void)
{
	YiTing* res = Nil<YiTing>();
	res = new YiTing;
	res->display();
	return res;
}

void YiTing::initialize(void)
{
	LifeForm::add_creator(YiTing::create, "YiTing");
}

void YiTing::spawn(void)
{
	my_reproduce_time = Event::now();
	YiTing* child = new YiTing;
	reproduce(child);
}

bool YiTing::checkSpawn(void)
{
	return (Event::now() - my_reproduce_time > min_reproduce_time && 
		health() * start_energy > 4 * (min_energy + eat_cost_function(0.0, 0.0)));
}

Action YiTing::encounter(const ObjInfo& that)
{
	myName = that.species;

	myDelete(this->huntEvent);

	if (!checkEnemy(that.their_speed))
	{
		set_course(that.bearing + M_PI);
		myDelete(huntEvent);
		huntEvent = new Event(0.5, make_procedure(*this, &YiTing::hunt));
		return LIFEFORM_IGNORE;
	}
	else
	{
		double winProb = eat_success_chance(this->health(), that.health);
		double loseProb = eat_success_chance(that.health, this->health());
		double trueWP = 0.0;
		double trueLP = 0.0;
		
		double spawnWP = eat_success_chance(this->health() * (1 - reproduce_cost) / 2, that.health);
		double spawnLP = eat_success_chance(that.health, this->health() * (1 - reproduce_cost) / 2);
		double trueSWP = 0.0;
		double trueSLP = 0.0;
		double expectedGain = 0.0;
		double expectedSGain = 0.0;
		double huntSpeed = 0.0;
		double escapeSpeed = 0.0;

		switch (encounter_strategy)
		{
			case BIG_GUY_WINS:
				if (this->health() >= that.health)
				{
					trueWP = winProb;
					trueLP = loseProb - winProb * loseProb;
				}
				else
				{
					trueWP = winProb - winProb * loseProb;
					trueLP = loseProb;
				}
				expectedGain = that.health * trueWP - this->health() * trueLP;

				if (this->health() * (1 - reproduce_cost) / 2 >= that.health)
				{
					trueSWP = spawnWP;
					trueSLP = spawnLP - spawnWP * spawnLP;
				}
				else
				{
					trueSWP = spawnWP - spawnWP * spawnLP;
					trueSLP = spawnLP;
				}
				expectedSGain = that.health * trueSWP - this->health() * trueSLP / 2;

				huntSpeed = max_speed - 0.1 + myTailSpeed;
				escapeSpeed = max_speed - 0.1 + myTailSpeed;
				strategyFirst(winProb, expectedGain, spawnWP, expectedSGain, huntSpeed, escapeSpeed, that);
				break;
			case UNDERDOG_IS_HERE:
				if (this->health() <= that.health)
				{
					trueWP = winProb;
					trueLP = loseProb - winProb * loseProb;
				}
				else
				{
					trueWP = winProb - winProb * loseProb;
					trueLP = loseProb;
				}
				expectedGain = that.health * trueWP - this->health() * trueLP;

				if (this->health() * (1 - reproduce_cost) / 2 <= that.health)
				{
					trueSWP = spawnWP;
					trueSLP = spawnLP - spawnWP * spawnLP;
				}
				else
				{
					trueSWP = spawnWP - spawnWP * spawnLP;
					trueSLP = spawnLP;
				}
				expectedSGain = that.health * trueSWP - this->health() * trueSLP / 2;

				huntSpeed = max_speed - 0.1 + myTailSpeed;
				escapeSpeed = max_speed - 0.1 + myTailSpeed;
				strategyFirst(winProb, expectedGain, spawnWP, expectedSGain, huntSpeed, escapeSpeed, that);
				break;
			case FASTER_GUY_WINS:
				trueWP = winProb;
				trueLP = loseProb - winProb * loseProb;
				expectedGain = that.health * trueWP - this->health() * trueLP;

				trueSWP = spawnWP;
				trueSLP = spawnLP - spawnWP * spawnLP;
				expectedSGain = that.health * trueSWP - this->health() * trueSLP / 2;

				huntSpeed = max_speed;
				escapeSpeed = max_speed;
				strategyFirst(winProb, expectedGain, spawnWP, expectedSGain, huntSpeed, escapeSpeed, that);
				break;
			case SLOWER_GUY_WINS:
				trueWP = winProb - winProb * loseProb;
				trueLP = loseProb;
				expectedGain = that.health * trueWP - this->health() * trueLP;

				trueSWP = spawnWP - spawnWP * spawnLP;
				trueSLP = spawnLP;
				expectedSGain = that.health * trueSWP - this->health() * trueSLP / 2;

				huntSpeed = myTailSpeed;
				escapeSpeed = max_speed - 0.1 + myTailSpeed;
				strategyFirst(winProb, expectedGain, spawnWP, expectedSGain, huntSpeed, escapeSpeed, that);
				break;
			default:
				trueWP = winProb - winProb * loseProb / 2;
				trueLP = loseProb - winProb * loseProb / 2;
				expectedGain = that.health * trueWP - this->health() * trueLP;

				trueSWP = spawnWP - spawnWP * spawnLP / 2;
				trueSLP = spawnLP - spawnWP * spawnLP / 2;
				expectedSGain = that.health * trueSWP - this->health() * trueSLP / 2;

				huntSpeed = max_speed - 0.1 + myTailSpeed;
				escapeSpeed = max_speed - 0.1 + myTailSpeed;
				strategyFirst(winProb, expectedGain, spawnWP, expectedSGain, huntSpeed, escapeSpeed, that);
				break;
		}

		return LIFEFORM_EAT;
	}
}

void YiTing::strategyFirst(double winProb, double expectedGain, 
						   double spawnWP, double expectedSGain,
						   double huntSpeed, double escapeSpeed, const ObjInfo& that)
{
	if (spawnWP < 0.11 && checkSpawn())
	{
		spawn();
		set_speed(huntSpeed);
		set_course(that.bearing);
		myDelete(huntEvent);
		huntEvent = new Event(0.2, make_procedure(*this, &YiTing::hunt));
	}
	else if (expectedSGain > expectedGain && checkSpawn())
	{
		spawn();
		if (expectedSGain >= 0)
		{
			set_speed(huntSpeed);
			set_course(that.bearing);
			myDelete(huntEvent);
			huntEvent = new Event(0.2, make_procedure(*this, &YiTing::hunt)); 
		}
		else
		{
			set_speed(escapeSpeed);
			set_course(that.bearing + M_PI / 2);
			myDelete(huntEvent);
			huntEvent = new Event(0.4, make_procedure(*this, &YiTing::hunt));
		}
	}
	else if (expectedGain >= 0 || winProb < 0.11)
	{
		set_speed(huntSpeed);
		set_course(that.bearing);
		myDelete(huntEvent);
		huntEvent = new Event(0.2, make_procedure(*this, &YiTing::hunt));
	}
	else
	{
		set_speed(escapeSpeed);
		set_course(that.bearing + M_PI / 2);
		myDelete(huntEvent);
		huntEvent = new Event(0.4, make_procedure(*this, &YiTing::hunt));
	}
}

bool YiTing::checkEnemy(double speed)
{
	int integralPart = 0;
	while((double)(integralPart) <= speed)
		++integralPart;
	--integralPart;
	speed = speed - (double)(integralPart);
	if(fabs(speed - myTailSpeed) < 0.00001)
		return false;
	else
		return true;
}

void YiTing::hunt(void)
{
	ObjList prey = perceive(20.0);
	if (prey.size() > 0)
	{
		sort(prey.begin(), prey.end(), myComp());
		if(!huntPrey(prey))
		{
			updateName(prey);
			set_speed(1.0 + myTailSpeed);
			myDelete(huntEvent);
			huntEvent = new Event(2.0, make_procedure(*this, &YiTing::hunt));
		}
	}
	else
	{
		set_course(get_course() + M_PI);
		set_speed(2.0 + myTailSpeed);
		myDelete(huntEvent);
		huntEvent = new Event(10.0, make_procedure(*this, &YiTing::hunt));
	}
}

void YiTing::updateName(const ObjList& prey)
{
	map<string, int> nameMap;

	for(ObjList::const_iterator thePrey = prey.begin(); thePrey != prey.end(); ++thePrey)
	{
		map<string, int>::iterator it = nameMap.find(thePrey->species) ;
		if (it == nameMap.end())
			nameMap.insert(pair<string, int>(thePrey->species, 0));
		else
			++it->second;
	}
	
	int maxNum = 0;
	for(map<string, int>::iterator it = nameMap.begin(); it != nameMap.end(); ++it)
	{
		if (it->second > maxNum)
		{
			maxNum = it->second;
			myName = it->first;
		}
	}
}

bool YiTing::huntPrey(const ObjList& prey)
{
	const String fav_food = "Algae";

	for(ObjList::const_iterator thePrey = prey.begin(); thePrey != prey.end(); ++thePrey)
	{
		if (thePrey->their_speed == 0 && thePrey->species == fav_food)
		{
			myName = thePrey->species;
			set_course(thePrey->bearing);
			double curSpeed = 0.0;
			if (encounter_strategy == FASTER_GUY_WINS)
			{
				curSpeed = max_speed - 1 + myTailSpeed;
				set_speed(max_speed - 1 + myTailSpeed);
			}
			else if (encounter_strategy == SLOWER_GUY_WINS)
			{
				curSpeed = 1 + myTailSpeed;
				set_speed(1 + myTailSpeed);
			}
			else
			{
				curSpeed = 5 + myTailSpeed;
				set_speed(5 + myTailSpeed);
			}
			myDelete(huntEvent);
			huntEvent = new Event(thePrey->distance/ curSpeed + 0.1, make_procedure(*this, &YiTing::hunt));
			return true;
		}

		if (checkEnemy(thePrey->their_speed) && canEat(*thePrey))
		{
			myName = thePrey->species;
			set_course(thePrey->bearing);
			double curSpeed = 0.0;
			if (encounter_strategy == FASTER_GUY_WINS)
			{
				curSpeed = max_speed - 1 + myTailSpeed;
				set_speed(max_speed - 1 + myTailSpeed);
			}
			else if (encounter_strategy == SLOWER_GUY_WINS)
			{
				curSpeed = 1 + myTailSpeed;
				set_speed(1 + myTailSpeed);
			}
			else
			{
				curSpeed = 5 + myTailSpeed;
				set_speed(5 + myTailSpeed);
			}
			myDelete(huntEvent);
			huntEvent = new Event(thePrey->distance/ curSpeed, make_procedure(*this, &YiTing::hunt));
			return true;
		}
	}

	return false;
}

bool YiTing::canEat(const ObjInfo& that)
{
	double winProb = eat_success_chance(this->health(), that.health);
	double loseProb = eat_success_chance(that.health, this->health());
	double trueWP = 0.0;
	double trueLP = 0.0;
	
	double expectedGain = 0.0;
	double fastSpeed = max_speed - 1 + myTailSpeed;
	double slowSpeed = 1 + myTailSpeed;

	switch (encounter_strategy)
	{
		case BIG_GUY_WINS:
			if (this->health() >= that.health)
			{
				trueWP = winProb;
				trueLP = loseProb - winProb * loseProb;
			}
			else
			{
				trueWP = winProb - winProb * loseProb;
				trueLP = loseProb;
			}
			expectedGain = that.health * trueWP - this->health() * trueLP;
			break;
		case UNDERDOG_IS_HERE:
			if (this->health() <= that.health)
			{
				trueWP = winProb;
				trueLP = loseProb - winProb * loseProb;
			}
			else
			{
				trueWP = winProb - winProb * loseProb;
				trueLP = loseProb;
			}
			expectedGain = that.health * trueWP - this->health() * trueLP;
			break;
		case FASTER_GUY_WINS:
			if (fastSpeed >= that.their_speed)
			{
				trueWP = winProb;
				trueLP = loseProb - winProb * loseProb;
			}
			else
			{
				trueWP = winProb - winProb * loseProb;
				trueLP = loseProb;
			}
			expectedGain = that.health * trueWP - this->health() * trueLP;
			break;
		case SLOWER_GUY_WINS:
			if (slowSpeed <= that.their_speed)
			{
				trueWP = winProb;
				trueLP = loseProb - winProb * loseProb;
			}
			else
			{
				trueWP = winProb - winProb * loseProb;
				trueLP = loseProb;
			}
			expectedGain = that.health * trueWP - this->health() * trueLP;
			break;
		default:
			trueWP = winProb - winProb * loseProb / 2;
			trueLP = loseProb - winProb * loseProb / 2;
			expectedGain = that.health * trueWP - this->health() * trueLP;
			break;
	}

	if (expectedGain > 0)
		return true;
	else
		return false;
}
		