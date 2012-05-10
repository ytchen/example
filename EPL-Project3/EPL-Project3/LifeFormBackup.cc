/*
 * LifeForm.cc -- The student half of the LifeForm class implementation
 * See LifeForm.h for details (and the project instructions)
 *
 * My Name Here
 * Today's Date Here
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

using namespace std;

void LifeForm::age(void)
{
	if(!is_alive)
		return ;

	energy-= age_penalty;
	if(energy<min_energy){
	//	is_alive=false;  
		die();
	}
	else
		(void) new Event(age_frequency, make_procedure(*this, &LifeForm::age));


}

void LifeForm::set_course(double ce){

	if(!is_alive)
		return ;

	Delete(border_cross_event);
	LifeForm::course=ce;
	compute_next_move();

}
void LifeForm::set_speed(double sd){
	if(!is_alive)
		return ;

	Delete(border_cross_event);
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
//	_DPRINTF(("[hw2] info this x%f y%f  that x%f y%f ",pos.xpos,pos.ypos,that->position().xpos,that->position().ypos));

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
		//is_alive=false;
		die();
		return objvect;
	}
	vector<LifeForm*> vect =  space.nearby(pos,dPR);
	for(vector<LifeForm*>::iterator it= vect.begin(); it!=vect.end();it++){

		if(this!=(*it))
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

	double dDif=Event::now()-update_time;
	update_time=Event::now();
	Point PosNew(0.0,0.0);
	if(dDif>0.001){
		PosNew.xpos= pos.xpos+ (speed*cos(course)*dDif);   //x-direction movement
		PosNew.ypos= pos.ypos+ (speed*sin(course)*dDif);   //y-direction movement
		space.update_position(pos,PosNew);             // update info in QuadTree
		pos=PosNew;
		energy-=movement_cost(speed,dDif);				//subtract movement cost

		if(energy<min_energy||space.is_out_of_bounds(PosNew)){
			//is_alive=false;
			die();

		}
	}

}
void LifeForm::compute_next_move(void){
	if(!is_alive)
		return ;

	double dist=space.distance_to_edge(pos,course);
	double dt=dist/speed;
	dt+=Point::tolerance;  //make sure the object is cross the boundary in the next border_cross event  
	border_cross_event=new Event(dt,make_procedure(*this,&LifeForm::border_cross));  //schedule border_cross event

}
void LifeForm::border_cross(void){
	//
	update_position();
	check_encounter();
	compute_next_move();


}
void LifeForm::check_encounter(void){
	if(!is_alive)
		return ;

	LifeForm* pTarget= space.closest(pos);

	//if(pos.xpos == pTarget->pos.xpos&&pos.ypos==pTarget->pos.ypos)
	//	return;
	if(pTarget&&pTarget->is_alive){

		double dist(0.0);
		dist+= (pos.xpos-pTarget->pos.xpos)*(pos.xpos-pTarget->pos.xpos);
		dist+= (pos.ypos-pTarget->pos.ypos)*(pos.ypos-pTarget->pos.ypos);
		dist=sqrt(dist);
		if(dist<encounter_distance){
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
	(void) new Event(digestion_time, make_procedure(*this,&LifeForm::gain_energy).bind(Food->energy));
	Food->die();
}
void LifeForm::resolve_encounter(LifeForm* that){
	Action xa=this->encounter(this->info_about_them(that));
	Action ya=that->encounter(that->info_about_them(this));

	LifeForm *Eater(NULL),*Food(NULL);
	double dEat(0.0);

	if (xa==LIFEFORM_IGNORE && ya==LIFEFORM_IGNORE){
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

	Delete(border_cross_event);
	update_position();
	// new Event(min_delta_time,make_procedure(*this,&LifeForm::update_position));  //schedule update_position event
	 new Event(min_delta_time*2,make_procedure(*this,&LifeForm::compute_next_move));  //schedule update_position event
}
void LifeForm::reproduce(LifeForm* obj){


	LifeForm* nearest;
	do {
		obj->pos.ypos = drand48() * grid_max / 2.0 + grid_max / 4.0;
		obj->pos.xpos = drand48() * grid_max / 2.0 + grid_max / 4.0;
		if (num_life > 1)
			nearest = space.closest(obj->pos);
		else
			nearest = Nil<LifeForm>();
	} while (nearest && nearest->position().distance(obj->position())
		<= encounter_distance);
	obj->start_point = obj->pos;
	space.insert(obj, obj->pos,
		make_procedure(*obj, &LifeForm::region_resize));
	(void) new Event(age_frequency, make_procedure(*obj, &LifeForm::age));
	obj->is_alive = true;
}