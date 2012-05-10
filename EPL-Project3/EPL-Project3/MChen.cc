#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "MChen.h"
#include "Window.h"
//#include "debug.h"

using namespace std;
#if !defined(Delete)
#define Delete(x) (delete (x), (x) = 0)
#endif


#if !defined(myDelete)
#define myDelete(event) { if (event!=NULL&&event->now()!=Event::now()) { Delete(event); } }
#endif

#if !defined(NORMAL_PI)
inline double normalPI(double pi){
	if(pi<0)
		pi+=M_PI*2;
	else if(pi>2*M_PI){
		pi-=2*M_PI;
	}
	return pi;

}
#endif

Initializer<MChen> __MChen_initializer;

string MChen::species_name(void) const
{
	if(bAttack)
		return "Algae";
	else
		return "MChen";
}
string MChen::player_name(void) const
{
	return "MChen";
}

Action MChen::encounter(const ObjInfo& info)
{

	if(HitObj==info){
		countADhunt++;
		//_DPRINTF(("[MChen] Event ad %d  success %d",countAD,countADhunt));
	}

	HitObj=info;
	if (info.species == species_name()) {
		/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		hunt_event = new Event(10.1, make_procedure(*this, &MChen::hunt));
		return LIFEFORM_IGNORE;
	}
	//else if(info.species=="Algae" && info.their_speed!=0){   
	//	set_course(info.bearing + M_PI);
	//	hunt_event = new Event(10.2, make_procedure(*this, &MChen::hunt));
	//	return LIFEFORM_IGNORE;
	//}
	else {
		myDelete( hunt_event);
		if(encounter_strategy==BIG_GUY_WINS){

			hunt_event = new Event(10, make_procedure(*this, &MChen::hunt));
		}
		else if(encounter_strategy==UNDERDOG_IS_HERE){
			if(health()<info.health){
				hunt_event = new Event(0.4, make_procedure(*this, &MChen::HuntEncounter));
			}
			else{

				if(health()*0.5<info.health&&health()>1.0){
					//spawn();
					MChen* child = new MChen;
					reproduce(child);
					hunt_event = new Event(0.4, make_procedure(*this, &MChen::HuntEncounter));
				}
				else{
					set_speed(max_speed);
					myspeed=max_speed;
					set_course(info.bearing + M_PI);  //run
					mycourse=info.bearing + M_PI;
					hunt_event = new Event(8, make_procedure(*this, &MChen::hunt));
				}


			}
		}
		else if(encounter_strategy==FASTER_GUY_WINS){
					myspeed=max_speed;	
			set_speed(myspeed);

			//myDelete(hunt_event_encounter);
			hunt_event = new Event(10.0, make_procedure(*this, &MChen::hunt));
		}
		else if(encounter_strategy==SLOWER_GUY_WINS){
			myspeed=0.0;
			set_speed(-20.0);

			hunt_event = new Event(min_delta_time, make_procedure(*this, &MChen::HuntEncounter));
		}
		else {
			if(health()>info.health){
					hunt_event = new Event(0.4, make_procedure(*this, &MChen::HuntEncounter));
			}
			else

			hunt_event = new Event(10.0, make_procedure(*this, &MChen::hunt));

			if (health() >= reproduceC) spawn();
		}
		//myDelete( hunt_event);
		//hunt_event = new Event(10.0, make_procedure(*this, &MChen::hunt));
		return LIFEFORM_EAT;
	}
}

void MChen::initialize(void)
{
	LifeForm::add_creator(MChen::create, "MChen");
}

/*
* REMEMBER: do not call set_course, set_speed, perceive, or reproduce
* from inside the constructor!!!!
* you must wait until the object is actually alive
*/
MChen::MChen()
{
	hunt_event = Nil<Event>();
	(void) new Event(0.0, make_procedure(*this, &MChen::live));
	Tsplit = 1 ;
	countADhunt=0;
	HitObj =ObjInfo();
	countAD=0;
	dTimeFre=10.0;
	dTimeSS=-dTimeFre;
	bAttack=false;
	if(encounter_strategy==BIG_GUY_WINS){
		reproduceC=30.0;
	}
	else if(encounter_strategy==UNDERDOG_IS_HERE){
		reproduceC=2.0;
	}
	else
	reproduceC=4.0;
	evalution=1.0;
	attackercount=0;
}


MChen::~MChen()
{
}

void MChen::spawn(void)
{
	//if(!bAttack)
	//	bAttack=drand48()>0.4?true:false;

	if(bAttack){
		if(health()>reproduceC*2){
			MChen* child = new MChen;
			reproduce(child);
		}
	}
	else{
		MChen* child = new MChen;
		reproduce(child);
	}
}


Color MChen::my_color(void) const
{
	return CYAN;
}  

LifeForm* MChen::create(void)
{
	MChen* res = Nil<MChen>();
	res = new MChen;
	res->display();
	return res;
}


/*
* this event is only called once, when the object is "born"
*
* I don't want to call "set_course" and "set_speed" from the constructor
* 'cause OBJECTS ARE NOT ALIVE WHEN THEY ARE CONSTRUCTED (remember!)
* so... in the constructor I schedule this event to happen in 0 time
* units
*/
void MChen::live(void)
{
	mycourse=drand48() * 2.0 * M_PI;
	set_course(mycourse);
	myspeed=2 + 5.0 * drand48();
	set_speed(myspeed);
	hunt_event = new Event(1.0, make_procedure(*this, &MChen::hunt));
}
void MChen::HuntEncounter(void)
{
	countAD++;


	hunt_event = Nil<Event>();
	ObjList prey = perceive(6.0);

	double best_d = HUGE;
	int count = 0 ;
	for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
		if ((*i)==HitObj&&HitObj.their_speed<4) {
			set_course((*i).bearing);
			set_speed(max_speed);
			myspeed=max_speed;
			mycourse=(*i).bearing;

		}
	}
	hunt_event = new Event(1.5, make_procedure(*this, &MChen::hunt));



}
void MChen::hunt(void)
{

	//if(Event::now()-dTimeSS>=10.0){ //search for group avoid leaving center
	//	dTimeSS=Event::now();
	//	ST(true);
	//	return;
	//}

	if(encounter_strategy==FASTER_GUY_WINS  ){
		myspeed=5.0+drand48()*5.0;
	}
	else if(encounter_strategy==SLOWER_GUY_WINS||encounter_strategy==BIG_GUY_WINS){
		myspeed=1.0+drand48()*3.0;
	}
	else{
		myspeed=3.0+drand48()*4.0;
	}

	const string fav_food = "Algae";
	
	
	hunt_event = Nil<Event>();
	
	if(!bAttack){

		ObjList prey = perceive(25.0);

		double best_d = HUGE;
		int count = 0 ;
		double dt(10.0);
		for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
			count++ ;
			if ((*i).species == fav_food && (*i).their_speed==0 ) {
				course_changed = 0 ;
				if (best_d > (*i).distance) {
					set_course((*i).bearing);
					mycourse=(*i).bearing;
					best_d = (*i).distance;
				}
			}
		}
		if(best_d == HUGE){
			ST(true);
			return;
		}
		dt=best_d/myspeed;
		dt=9>dt?9:dt;
		hunt_event = new Event(dt, make_procedure(*this, &MChen::hunt));
	

		if (health() >= reproduceC) spawn();
	}
	else{ //attacker
		ObjList prey = perceive(25.0);

		double best_d = HUGE;
		int count = 0 ;
		ObjInfo target=ObjInfo();
		double ds(0.0);
		for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
			count++ ;
			if (/*(*i).species != fav_food &&*/ (*i).species!=player_name() ) {
				//	ds=(*i).distance*  cos((*i).bearing-(*i).their_course); // prefer 
				if(encounter_strategy==BIG_GUY_WINS){
					if(health()>(*i).health){
						ds=(*i).distance;
						//_DPRINTF(("[MChen] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
						course_changed = 0 ;
						if (best_d > ds) {
							set_course((*i).bearing);
							mycourse=(*i).bearing;
							best_d = (*i).distance;
						}
					}
				}
				else if(encounter_strategy==UNDERDOG_IS_HERE){
					if(health()<(*i).health){
						ds=(*i).distance;
						//_DPRINTF(("[MChen] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
						course_changed = 0 ;
						if (best_d > ds) {
							set_course((*i).bearing);
							mycourse=(*i).bearing;
							best_d = (*i).distance;
						}
					}

				}
				else{
					ds=(*i).distance;
					//_DPRINTF(("[MChen] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
					course_changed = 0 ;
					if (best_d > ds) {
						set_course((*i).bearing);
						mycourse=(*i).bearing;
						best_d = (*i).distance;
					}
				}

			}
		}
		if(best_d == HUGE){
			ST(true);
			return;
		}else{
		
		//evalution=1.0;
		//attackercount=0;
		}
		double dt(0.0);
		dt=best_d/myspeed;
		dt=12>dt?12:dt;
		hunt_event = new Event(dt, make_procedure(*this, &MChen::hunt));

		if (health() >= reproduceC) spawn();
	}


}
void MChen::ST(bool bspe){
	double ds(0.0);
	const string fav_food = "Algae";

	//_DPRINTF(("[Mchen]Large search"));
	hunt_event = Nil<Event>();
	ObjList prey = perceive(100.0*evalution);

	double degree=4/M_PI;
	int direction[8],spedirection[8];
	double dist[8],spedist[8];
	double CR[8],speCR[8];
	memset(direction,0,sizeof(int)*8);
	memset(spedirection,0,sizeof(int)*8);
	memset(CR,0,sizeof(double)*8);
	memset(speCR,0,sizeof(double)*8);
	//memset(dist,1000,sizeof(double)*8);
	//memset(spedist,1000,sizeof(double)*8);

	for(int i=0;i<8;i++){
		dist[i]=spedist[i]=10000.0;
	}
	int count = 0 ;



	int Dit;
	for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
		count++ ;
		if (bspe && (*i).species == fav_food && (*i).their_speed==0 ) { // Algae

			double angle =normalPI((*i).bearing);
			Dit= int(angle*degree);
			spedirection[Dit]++;
			if (spedist[Dit] > (*i).distance) {
				speCR[Dit]=(*i).bearing;
				spedist[Dit] = (*i).distance;
			}
		}
		else if((*i).species!=player_name()){ // non myself and Algae
			if(encounter_strategy==BIG_GUY_WINS){
				if(health()>(*i).health){
					double angle =normalPI((*i).bearing);
					Dit= int(angle*degree);
					direction[Dit]++;
					if (dist[Dit] > (*i).distance) {
						CR[Dit]=(*i).bearing;
						dist[Dit] = (*i).distance;
					}
				}
			}
			else if(encounter_strategy==UNDERDOG_IS_HERE){
				if(health()<(*i).health){
					double angle =normalPI((*i).bearing);
					Dit= int(angle*degree);
					direction[Dit]++;
					if (dist[Dit] > (*i).distance) {
						CR[Dit]=(*i).bearing;
						dist[Dit] = (*i).distance;
					}
				}

			}
			else{
				double angle =normalPI((*i).bearing);
				Dit= int(angle*degree);
				direction[Dit]++;
				if (dist[Dit] > (*i).distance) {
					CR[Dit]=(*i).bearing;
					dist[Dit] = (*i).distance;
				}
			}

		}
	}

	int speMax(0),genMax(0),Index(0),speIndex(0);
	for(int i=0;i<8;i++){
		if(speMax<spedirection[i]){
			speMax=spedirection[i];
			speIndex=i;
		}
		if(genMax<direction[i]){
			genMax=direction[i];
			Index=i;
		}
	}
	if(bspe&&speMax>0){
		attackercount=0;
		evalution=1;
		set_course(speCR[speIndex]) ;
		mycourse=speCR[speIndex];
		double dt=(spedist[speIndex])/myspeed;
		if(dt>dTimeFre)
			dt=dTimeFre;
		//_DPRINTF(("[Mchen]Large search max %d index:%d crouse %f",speMax,speIndex,speCR[speIndex]));
		hunt_event = new Event(10, make_procedure(*this, &MChen::hunt));
	}
	else if(genMax>0){   //hard time
		attackercount++;
		evalution*=0.8;
		bAttack=true;
		myspeed*=evalution;
		set_speed(myspeed);
		set_course(CR[Index]) ;
		mycourse=CR[Index];
		double dt=(dist[Index])/myspeed;
		if(dt>dTimeFre)
			dt=dTimeFre;
		//_DPRINTF(("[Mchen]Large search Max%d index:%d dist %f",genMax,Index,CR[Index]));
		hunt_event = new Event((10+5*attackercount), make_procedure(*this, &MChen::hunt));
	}
	else{
		attackercount++;
		evalution*=0.5;
		bAttack=true;
		if(course_changed == 0){
			course_changed = 1 ;
			set_course(get_course() + M_PI) ;
		}
		myspeed*=evalution;
		set_speed(myspeed);
		hunt_event = new Event((10+5*attackercount), make_procedure(*this, &MChen::hunt));
	}

}

