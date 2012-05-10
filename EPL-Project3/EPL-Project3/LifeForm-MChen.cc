/*
 * LifeForm.cc -- The student half of the LifeForm class implementation
 * See LifeForm.h for details (and the project instructions)
 *
 * Name:MingJun Chen
 * EID:mc36898
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

const double min_update_time=0.001;

#if !defined(Delete)
#define Delete(x) (delete (x), (x) = 0)
#endif


#if !defined(myDelete)
#define myDelete(event) { if (event!=NULL&&event->now()!=Event::now()) { Delete(event); } }
#endif


using namespace std;
void LifeForm::age(void)
{
	if(!is_alive)
		return ;
	//_DPRINTF(("[hw2]age  %f ",Event::now()));
	energy-= age_penalty;
	if(energy<min_energy){
		(void) new Event(0.0, make_procedure(*this,&LifeForm::die).bind(true));
	}
	else
		(void) new Event(age_frequency, make_procedure(*this, &LifeForm::age));


}

void LifeForm::set_course(double ce){

	if(!is_alive)
		return ;

	myDelete(border_cross_event);

	double TempTime(update_time);
	update_position();

	LifeForm::course=ce;
	compute_next_move();
}
void LifeForm::set_speed(double sd){
	if(!is_alive)
		return ;
	//_DPRINTF(("[hw2]set_speed  %f ",Event::now()));
	myDelete(border_cross_event);	


	double TempTime(update_time);
	update_position();

		sd=sd>0?sd:0;
	sd=sd<max_speed?sd:max_speed;
	LifeForm::speed=sd;
	compute_next_move();

}
ObjInfo LifeForm::info_about_them(const LifeForm* that) const {

	ObjInfo obj;
	obj.health=that->health();
	obj.their_course =that->get_course();
	obj.their_speed=that->get_speed();
	obj.species=that->species_name();
	obj.bearing= this->pos.bearing(that->position());   // different of courses between two LifeForm
	obj.distance= this->pos.distance(that->position()); // distance between two LifeForm

	return  obj;
}
ObjList LifeForm::perceive(double drange){

	if(!is_alive)
		return  ObjList();

	ObjList objvect;
	objvect.clear();

	double dPR = drange < min_perceive_range ? min_perceive_range : drange;
	dPR=drange > max_perceive_range ? max_perceive_range : drange;
	energy-= perceive_cost(dPR);
	if(energy<min_energy){
		(void) new Event(0.0, make_procedure(*this,&LifeForm::die).bind(true));
		return objvect;
	}
	vector<LifeForm*> vect =  space.nearby(pos,dPR);
	for(vector<LifeForm*>::iterator it= vect.begin(); it!=vect.end();it++){
			objvect.push_back( info_about_them(*it) );
	}

	return objvect;


}
// calculate the current position for
// an object.  If less than Time::tolerance
// time units have passed since the last
// call to update_position, then do nothing
// (we can't have moved very far so there's
// no point in updating our position)

void LifeForm::update_position(void){
	if(!is_alive)
		return ;
	if(speed==0)
		return;

	//_DPRINTF(("[hw2]update_position in %f  %f %f",Event::now(),pos.xpos,pos.ypos));
	double dDif=Event::now()-update_time;
	update_time=Event::now();
	Point PosNew(0.0,0.0);
	if(dDif>min_update_time){
		PosNew.xpos= pos.xpos+ (speed*cos(course)*dDif);   //x-direction movement
		PosNew.ypos= pos.ypos+ (speed*sin(course)*dDif);   //y-direction movement
		energy-=movement_cost(speed,dDif);	
		if(energy<min_energy||space.is_out_of_bounds(PosNew)){
			(void) new Event(min_delta_time, make_procedure(*this,&LifeForm::die).bind(true));
			return;
		}
		space.update_position(pos,PosNew);             // update info in QuadTree
		pos=PosNew;
				//subtract movement cost
		//_DPRINTF(("[hw2]update_position  out %f  %f %f",Event::now(),pos.xpos,pos.ypos));

	}

}
void LifeForm::compute_next_move(void){
	if(!is_alive)
		return ;
	if(speed==0)
		return;


	double dist=space.distance_to_edge(pos,course);
	double dt=dist/speed;
	dt+=min_delta_time;  //make sure the object is cross the boundary in the next border_cross event  
	//_DPRINTF(("[hw2]compute_next_move %f move time %f dis:%f  speed%f",Event::now(),dt,dist,speed));
	if(dt<min_update_time){
		 dt=0.25/speed;
		dt+=min_delta_time;
	//	dt=0.05;  //make sure update postion next time;
		(void) new Event(dt,make_procedure(*this,&LifeForm::update_position));  //
		(void) new Event(dt,make_procedure(*this,&LifeForm::compute_next_move));  //
		return ;

	}
	////_DPRINTF(("[hw2]before compute_next_move move time %f dis:%f  speed%f  time %f",dt,dist,speed,Event::now()));
	border_cross_event=new Event(dt,make_procedure(*this,&LifeForm::border_cross));  //schedule border_cross event

	//_DPRINTF(("[hw2]Create border_cross_eventtime %f %d ",Event::now()+dt,border_cross_event));

}
void LifeForm::border_cross(void){
	//
	if(!is_alive)
		return ;
	if(speed==0)
		return;
	 double dDif=Event::now()-update_time;

	 if(dDif>min_update_time){
	update_position();

		compute_next_move();

	check_encounter();
	 }



}
void LifeForm::check_encounter(void){
	if(!is_alive)
		return ;

	LifeForm* pTarget= space.closest(pos);

	if(pTarget&&pTarget->is_alive){

		double dist(0.0);
		dist+= (pos.xpos-pTarget->pos.xpos)*(pos.xpos-pTarget->pos.xpos);
		dist+= (pos.ypos-pTarget->pos.ypos)*(pos.ypos-pTarget->pos.ypos);
		dist=sqrt(dist);


		double direction ;
		direction= fabs(course-pTarget->course);
		//_DPRINTF(("[hw2]check_encounter %f dis %f  %f",Event::now(),dist,fabs(direction-M_PI)));
		if(dist<=(encounter_distance) && fabs(direction-M_PI)>0.1){
			resolve_encounter(pTarget);
		}
	}
}
void  LifeForm::gain_energy(double dEn){
	if(!is_alive)
		return ;
	energy += eat_efficiency*dEn;

}
void LifeForm::eat(LifeForm* Food){

	if(!is_alive)
		return;
	//	_DPRINTF(("[hw2]eat %f eater %f %f  Food %f %f ",Event::now(), pos.xpos,pos.ypos,Food->pos.xpos,Food->pos.ypos));
	(void) new Event(digestion_time, make_procedure(*this,&LifeForm::gain_energy).bind(Food->energy));
	//gain_energy(Food->energy);
	//_DPRINTF(("[hw2]Delete eat  border_cross_event %d",Food->border_cross_event));
	myDelete(Food->border_cross_event);
	(void) new Event(0.0, make_procedure(*Food,&LifeForm::die).bind(true));

}
void LifeForm::resolve_encounter(LifeForm* that){


	if(!this->is_alive||!that->is_alive)  // one of then die, return
		return;

	LifeForm *Eater(NULL),*Food(NULL);	
	Eater =this ;
	Food =that ;

	Event* pEBR;	
	Event* pFBR;
	pEBR=this->border_cross_event;
	pFBR=that->border_cross_event;

	//_DPRINTF(("[hw2]resolve_encounter %f A start %f %f  b start %f %f",Event::now(),start_point.xpos,start_point.ypos,that->start_point.xpos,start_point.ypos));
	Action xa=this->encounter(this->info_about_them(that));
	Action ya=that->encounter(that->info_about_them(this));


	// reduce encounter energy
	energy-=encounter_penalty;
	that->energy-=encounter_penalty;
	
	// Die due to encounter
	if(energy<min_energy){
		(void) new Event(0.0, make_procedure(*this,&LifeForm::die).bind(true));
		return;
	}
	if(that->energy<min_energy){
		myDelete(that->border_cross_event);
		(void) new Event(0.0, make_procedure(*that,&LifeForm::die).bind(true));
		return;
	}
	double dEat(0.0);

	//double dt=1.0/speed;
	//dt+=min_delta_time;  //make sure the object is cross the boundary in the next border_cross event  
	//////_DPRINTF(("[hw2]compute_next_move %f move time %f dis:%f  speed%f",Event::now(),dt,dist,speed));
	//myDelete(border_cross_event);
	//border_cross_event=new Event(dt,make_procedure(*this,&LifeForm::border_cross));  //schedule border_cross even
	
	//to make sure encounter only once, schedule the next border_cross_event 
	if (xa==LIFEFORM_IGNORE && ya==LIFEFORM_IGNORE){
        //make sure only one encounter 
		return;
	}
	else if (xa==LIFEFORM_EAT && ya==LIFEFORM_IGNORE){
		Eater =this ;
		Food =that ;
	}
	else if (xa==LIFEFORM_IGNORE && ya==LIFEFORM_EAT){
		Eater=that;
		Food=this;
	}
	else{

		switch(encounter_strategy){
			case EVEN_MONEY: // flip a coin
				if(drand48()>0.5){
					Eater =this ;
					Food =that ;
				}
				else{
					Eater=that;
					Food=this;
				}
				break;
			case BIG_GUY_WINS:     // big guy gets first bite
				Eater=this->energy>that->energy?this:that;
				Food=this->energy<that->energy?this:that;
				break;
			case UNDERDOG_IS_HERE:// let the little guy have a chance
				Eater=this->energy<that->energy?this:that;
				Food=this->energy>that->energy?this:that;
				break;
			case FASTER_GUY_WINS:// speeding eagle gets the mouse
				Eater=this->speed>that->speed?this:that;
				Food=this->speed<that->speed?this:that;
				break;
			case SLOWER_GUY_WINS: // ambush!
				Eater=this->speed<that->speed?this:that;
				Food=this->speed>that->speed?this:that;
				break;
			default:
				Eater =this;
				Food =that;
				break;
		}

	}
	dEat=eat_success_chance(Eater->energy,Food->energy);
	double dT=drand48();
	if(dT<dEat){
		Eater->eat(Food);
	}



}
void LifeForm::region_resize(void){
		//_DPRINTF(("[hw2]region_resize %f ",Event::now()));

	if(!is_alive)
		return;
	myDelete(border_cross_event);
	//(void)new Event(0.0,make_procedure(*this,&LifeForm::update_position));  //schedule update_position event
	update_position();
	compute_next_move();

	//_DPRINTF(("[hw2]Delete region_resize time %f border_cross_event %d ",Event::now(),border_cross_event));

	// (void )new Event(0.0,make_procedure(*this,&LifeForm::compute_next_move));  //schedule compute_next_move

}
void LifeForm::reproduce(LifeForm* obj){
	if(!is_alive)
		return;

	if(Event::now()-reproduce_time<min_reproduce_time)
		return;

	//_DPRINTF(("[hw2]reproduce %f ",Event::now()));
	reproduce_time=Event::now();
	LifeForm* nearest;
	Point posNew;
	double shift=sqrt(reproduce_dist);
	do {
		if(drand48()>0.5)
			obj->pos.ypos = pos.ypos+drand48()*(shift);
		else
			obj->pos.ypos = pos.ypos-drand48()*(shift);
		if(drand48()>0.5)
			obj->pos.xpos = pos.xpos+drand48()*(shift);
		else
			obj->pos.xpos = pos.xpos-drand48()*(shift);
		if (num_life > 1)
			nearest = space.closest(obj->pos);
		else
			nearest = Nil<LifeForm>();

		posNew.xpos= obj->pos.xpos;
		posNew.ypos= obj->pos.ypos;
	} while (nearest && nearest->position().distance(obj->position())
		<= encounter_distance&& !space.is_out_of_bounds(posNew));

	energy= (energy*(1-reproduce_cost))*0.5;
	obj->energy=energy;                      // reduce energy

	if(energy<min_energy){
		myDelete(border_cross_event);
		(void) new Event(0.0, make_procedure(*this,&LifeForm::die).bind(true));
		obj->start_point = obj->pos;
		space.insert(obj, obj->pos,
			make_procedure(*obj, &LifeForm::region_resize));
		(void) new Event(age_frequency, make_procedure(*obj, &LifeForm::age));   // poor guy birth and die at the same time
		obj->is_alive = true;
		(void) new Event(0.0, make_procedure(*obj,&LifeForm::die).bind(true));
		return;
	}

	obj->start_point = obj->pos;
	space.insert(obj, obj->pos,
		make_procedure(*obj, &LifeForm::region_resize));
	(void) new Event(age_frequency, make_procedure(*obj, &LifeForm::age));
	obj->is_alive = true;

}