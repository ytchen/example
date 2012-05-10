/*
 * LifeForm.cc -- The student half of the LifeForm class implementation
 * See LifeForm.h for details (and the project instructions)
 *
 * My Name Here: Yi-Ting Chen
 * Today's Date Here: May 6, 2012
 */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "Procedure.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"
#include <cmath>

using namespace std;

void LifeForm::age(void)
{
	if(!is_alive)
		return;

	this->energy = this->energy - age_penalty;
	if(this->energy < min_energy)
		this->die();
	else
		new Event(age_frequency, make_procedure(*this, &LifeForm::age));
}

void LifeForm::gain_energy(double in)
{
	if(!is_alive)
		return;

	this->energy = this->energy + in * eat_efficiency;
}

void LifeForm::eat(LifeForm* that)
{
	if(!is_alive)
		return;

	this->energy = this->energy - eat_cost_function(this->energy, that->energy);
	if (this->energy < min_energy)
		this->die();
	else
		new Event(digestion_time, make_procedure(*this, &LifeForm::gain_energy).bind(that->energy));
	myDelete(that->border_cross_event);
	that->die();
}

ObjInfo LifeForm::info_about_them(const LifeForm* that) const
{
	ObjInfo thatInfo;
	thatInfo.species			= that->species_name();
	thatInfo.health			= that->health();
	thatInfo.distance		= this->pos.distance(that->pos);
	thatInfo.bearing			= this->pos.bearing(that->pos);
	thatInfo.their_speed	= that->speed;
	thatInfo.their_course = that->course;

	return thatInfo;
}

void LifeForm::set_course(double inCourse)
{
	if(!is_alive)
		return;

	myDelete(border_cross_event);
	this->update_position();
	this->course = inCourse;
	this->compute_next_move();
}

void LifeForm::set_speed(double inSpeed)
{
	if(!is_alive)
		return;

	myDelete(border_cross_event);
	this->update_position();
	this->speed = max(min(inSpeed, max_speed), 0.0);
	this->compute_next_move();
}

void LifeForm::compute_next_move(void)
{
	if(!is_alive || this->speed < 0.001)
		return;

	double dist = space.distance_to_edge(this->pos, this->course);
	double crossTime = dist/this->speed + Point::tolerance;
	myDelete(this->border_cross_event);
	this->border_cross_event = new Event(crossTime, make_procedure(*this, &LifeForm::border_cross));
}

void LifeForm::reproduce(LifeForm* that)
{
	if(!is_alive)
		return;

	if(Event::now() - this->reproduce_time < min_reproduce_time)
		return;

	bool isCrowd = true;
	for(int numTry = 0; numTry < 100; ++numTry)
	{
		double xDiff = drand48() * 5.0;
		double yDiff = drand48() * 5.0;
		if (rand() % 2 == 1)
			xDiff = -xDiff;
		if (rand() % 2 == 1)
			yDiff = -yDiff;
		if (xDiff * xDiff + yDiff * yDiff >= reproduce_dist * reproduce_dist ||
			xDiff * xDiff + yDiff * yDiff <= encounter_distance * encounter_distance)
			continue;
		that->pos.xpos = this->pos.xpos + xDiff;
		that->pos.ypos = this->pos.ypos + yDiff;
		if ( space.is_out_of_bounds(that->pos) )
			continue;
		LifeForm* nearestLife = space.closest(that->pos);
		if (that->pos.distance(nearestLife->pos) > encounter_distance)
		{
			isCrowd = false;
			break;
		}
	}

	if (isCrowd)
	{
		double xDiff;
		double yDiff;
		do
		{
			xDiff = drand48() * 5.0;
			yDiff = drand48() * 5.0;
			if (rand() % 2 == 1)
				xDiff = -xDiff;
			if (rand() % 2 == 1)
				yDiff = -yDiff;
			that->pos.xpos = this->pos.xpos + xDiff;
			that->pos.ypos = this->pos.ypos + yDiff;
			if ( space.is_out_of_bounds(that->pos) || space.is_occupied(that->pos) )
				continue;
		} while( xDiff * xDiff + yDiff * yDiff >= reproduce_dist * reproduce_dist);
	}

	that->start_point = that->pos;
	that->is_alive = true;
	space.insert(that, that->pos, make_procedure(*that, &LifeForm::region_resize));
	this->reproduce_time = Event::now();
	that->reproduce_time = Event::now();
	this->energy = this->energy * (1 - reproduce_cost) / 2;
	that->energy = this->energy;

	if (this->energy < min_energy)
	{
		myDelete(this->border_cross_event);
		this->die();
		myDelete(that->border_cross_event);
		that->die();
	}
	else
	{
		new Event(age_frequency, make_procedure(*that, &LifeForm::age));
	}
}

ObjList LifeForm::perceive(double radius)
{
	if(!is_alive)
		return ObjList();

	radius = min(max(radius, min_perceive_range), max_perceive_range);
	this->energy = this->energy - perceive_cost(radius);
	if ( energy < min_energy)
	{
		myDelete(this->border_cross_event);
		this->die();
		return ObjList();
	}

	this->update_position();
	vector<LifeForm*> nearObjs = space.nearby(this->pos, radius);
	ObjList retList(nearObjs.size());
	for(int index = 0; index < nearObjs.size(); ++index)
		retList[index] = this->info_about_them(nearObjs[index]);
	return retList;
}

void LifeForm::update_position(void)
{
	if(!is_alive)
		return;

	double timeDiff = Event::now() - this->update_time;
	if (timeDiff > 0.001)
	{
		this->update_time = Event::now();
		Point newPos;
		newPos.xpos = this->pos.xpos + (this->speed * cos(this->course) * timeDiff);
		newPos.ypos = this->pos.ypos + (this->speed * sin(this->course) * timeDiff);
		this->energy = this->energy - movement_cost(this->speed, timeDiff);
		if(space.is_out_of_bounds(newPos) || this->energy < min_energy)
		{
			myDelete(this->border_cross_event);
			this->die();
		}
		else
		{
			space.update_position(this->pos, newPos);
			this->pos = newPos;
		}
	}
}

void LifeForm::check_encounter(void)
{
	if (!is_alive)
		return;

	if (num_life > 1)
	{
		LifeForm* nearestLife = space.closest(this->pos);
		if (nearestLife->is_alive && 
			this->pos.distance(nearestLife->pos) < encounter_distance)
		{
			resolve_encounter(nearestLife);
		}
	}
}

void LifeForm::resolve_encounter(LifeForm* that)
{
	if (!this->is_alive || !that->is_alive)
		return;

	that->energy = that->energy - encounter_penalty;
	if (that->energy < min_energy)
	{
		myDelete(that->border_cross_event);
		that->die();
	}
	this->energy = this->energy - encounter_penalty;
	if (this->energy < min_energy)
	{
		myDelete(this->border_cross_event);
		this->die();
	}
	
	if (this->is_alive && that->is_alive)
	{
		Action thisAction = this->encounter(this->info_about_them(that));
		Action thatAction = that->encounter(that->info_about_them(this));

		bool thisWin = false;
		bool thatWin = false;
		double thisWinProb = eat_success_chance(this->energy, that->energy);
		double thatWinProb = eat_success_chance(that->energy, this->energy);
		if (thisAction == LIFEFORM_EAT && drand48() < thisWinProb)
			thisWin = true;
		if (thatAction == LIFEFORM_EAT && drand48() < thatWinProb)
			thatWin = true;

		if (thisWin && thatWin)
		{
			switch(encounter_strategy)
			{
			case EVEN_MONEY:
				if (drand48() <= 0.5)
					this->eat(that);
				else
					that->eat(this);
				break;
			case BIG_GUY_WINS:
				if (this->energy >= that->energy)
					this->eat(that);
				else
					that->eat(this);
				break;
			case UNDERDOG_IS_HERE:
				if (this->energy <= that->energy)
					this->eat(that);
				else
					that->eat(this);
				break;
			case FASTER_GUY_WINS:
				if (this->speed >= that->speed)
					this->eat(that);
				else
					that->eat(this);
				break;
			case SLOWER_GUY_WINS:
				if (this->speed <= that->speed)
					this->eat(that);
				else
					that->eat(this);
				break;
			default:
				this->eat(that);
			}
		}
		else if (thisWin)
		{
			this->eat(that);
		}
		else if (thatWin)
		{
			that->eat(this);
		}
	}
}

void LifeForm::region_resize(void)
{
	if (!is_alive)
		return;

	this->update_position();
	this->compute_next_move();
}

void LifeForm::border_cross(void)
{
	if (!is_alive)
		return;

	this->update_position();
	this->compute_next_move();
	this->check_encounter();
}