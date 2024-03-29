#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "MChenRock.h"
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
#if !defined(ENCRIPT)
const double enmaxsp=max_speed-0.01;
const double mycode =0.009567;
const double mycodecheck =0.010001;
inline double encripted(double sp){
	sp*=100.0;
	int mod(sp);
	double sd= (double)mod*0.01;

	sd+=mycode;
	return sd;

}
inline double decripted(double sp){
	double sd=sp;

	sp*=100.0;
	int mod(sp);
	double temp= (double)mod*0.01;

	sd-=temp;
	return sd;

}
inline bool my_verify(double sp){
	return fabs(sp-mycode)<mycodecheck;
}
#endif
Initializer<MChenRock> __MChenRock_initializer;

string MChenRock::species_name(void) const
{
	if(bAttack)
		return "Algae";
	else
		return "MChenRock";
}
string MChenRock::player_name(void) const
{
	return "MChenRock";
}

Action MChenRock::encounter(const ObjInfo& info)
{

	if(HitObj==info){
		countADhunt++;
		//_DPRINTF(("[MChenRock] Event ad %d  success %d",countAD,countADhunt));
	}

	HitObj=info;
	if (info.species == player_name()) {  //myslef   
		/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		hunt_event = new Event(10.1, make_procedure(*this, &MChenRock::hunt));
		return LIFEFORM_IGNORE;
	}
//	else if(/*info.species=="Algae" &&*/ my_verify(decripted(myspeed))){  //myslef   
//		set_course(info.bearing + M_PI);
//		hunt_event = new Event(10.2, make_procedure(*this, &MChenRock::hunt));
//		return LIFEFORM_IGNORE;
//	}
	else {
		myDelete( hunt_event);
		if(encounter_strategy==BIG_GUY_WINS){
			if(health()>info.health){
				hunt_event_encounter = new Event(0.4, make_procedure(*this, &MChenRock::HuntEncounter));
			}
			else{

				myspeed=encripted(enmaxsp);
				set_speed(myspeed);
				set_course(info.bearing + M_PI);  //run
				mycourse=info.bearing + M_PI;
				hunt_event = new Event(8, make_procedure(*this, &MChenRock::hunt));



			}

		}
		else if(encounter_strategy==UNDERDOG_IS_HERE){
			if(health()<info.health){
				hunt_event_encounter = new Event(0.4, make_procedure(*this, &MChenRock::HuntEncounter));
			}
			else{

				if(health()*0.5<info.health){
					//spawn();
					MChenRock* child = new MChenRock;
					reproduce(child);
					hunt_event_encounter = new Event(0.4, make_procedure(*this, &MChenRock::HuntEncounter));
				}
				else{
					myspeed=encripted(enmaxsp);
					set_speed(myspeed);
					set_course(info.bearing + M_PI);  //run
					mycourse=info.bearing + M_PI;
					hunt_event = new Event(8, make_procedure(*this, &MChenRock::hunt));
				}


			}
		}
		else if(encounter_strategy==FASTER_GUY_WINS){
			myspeed=max_speed;
			set_speed(myspeed);

			//myDelete(hunt_event_encounter);
			hunt_event_encounter = new Event(0.4, make_procedure(*this, &MChenRock::HuntEncounter));  //pursuit if enemy survied 
		}
		else if(encounter_strategy==SLOWER_GUY_WINS){
			myspeed=0.0;
			set_speed(-20.0);

			hunt_event_encounter = new Event(10, make_procedure(*this, &MChenRock::HuntEncounter)); //pursuit if enemy survied 
		}
		else {
			//if(health()>info.health){
			//		hunt_event = new Event(0.4, make_procedure(*this, &MChenRock::hunt));
			//}
			///else

			hunt_event = new Event(10.0, make_procedure(*this, &MChenRock::hunt));

			if (health() >= reproduceC) spawn();
		}
		//myDelete( hunt_event);
		//hunt_event = new Event(10.0, make_procedure(*this, &MChenRock::hunt));
		return LIFEFORM_EAT;
	}
}

void MChenRock::initialize(void)
{
	LifeForm::add_creator(MChenRock::create, "MChenRock");
}

/*
* REMEMBER: do not call set_course, set_speed, perceive, or reproduce
* from inside the constructor!!!!
* you must wait until the object is actually alive
*/
MChenRock::MChenRock()
{
	hunt_event = Nil<Event>();
	hunt_event_encounter = Nil<Event>();
	(void) new Event(0.0, make_procedure(*this, &MChenRock::live));
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
		reproduceC=3.0;
	}
	else
		reproduceC=4.0;
	evalution=1.0;
	attackercount=0;
}


MChenRock::~MChenRock()
{
}

void MChenRock::spawn(void)
{
	//if(!bAttack)
	//	bAttack=drand48()>0.4?true:false;

	if(bAttack){
		if(health()>reproduceC*2){
			MChenRock* child = new MChenRock;
			reproduce(child);
		}
	}
	else{
		MChenRock* child = new MChenRock;
		reproduce(child);
	}
}


Color MChenRock::my_color(void) const
{
	return YELLOW;
}  

LifeForm* MChenRock::create(void)
{
	MChenRock* res = Nil<MChenRock>();
	res = new MChenRock;
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
void MChenRock::live(void)
{
	mycourse=drand48() * 2.0 * M_PI;
	set_course(mycourse);
	myspeed=2 + 5.0 * drand48();
myspeed=encripted(myspeed);
	set_speed(myspeed);
	hunt_event = new Event(1.0, make_procedure(*this, &MChenRock::hunt));
}
void MChenRock::HuntEncounter(void)
{
	countAD++;


	//hunt_event = Nil<Event>();
	//myDelete(hunt_event_encounter);
	ObjList prey = perceive(6.0);

	double best_d = HUGE;
	int count = 0 ;
	for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {
		if ((*i)==HitObj&&HitObj.their_speed<4&& !my_verify(decripted(myspeed))) {
			set_course((*i).bearing);
			set_speed(max_speed);
			myspeed=max_speed;
			mycourse=(*i).bearing;

		}
	}
	hunt_event = new Event(1.5, make_procedure(*this, &MChenRock::hunt));



}
void MChenRock::hunt(void)
{

	//if(Event::now()-dTimeSS>=10.0){ //search for group avoid leaving center
	//	dTimeSS=Event::now();
	//	ST(true);
	//	return;
	//}

	SetSpeed();
	set_speed(myspeed);

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
		hunt_event = new Event(dt, make_procedure(*this, &MChenRock::hunt));
	

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
			if (!my_verify(decripted((*i).their_speed) )&& (*i).species!=player_name() ) { 
				//	ds=(*i).distance*  cos((*i).bearing-(*i).their_course); // prefer 
				if(encounter_strategy==BIG_GUY_WINS){
					if(health()>(*i).health/*&& (*i).health>0.4*/){
						ds=(*i).distance;
						//_DPRINTF(("[MChenRock] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
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
						//_DPRINTF(("[MChenRock] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
						course_changed = 0 ;
						if (best_d > ds) {
							set_course((*i).bearing);
							mycourse=(*i).bearing;
							best_d = (*i).distance;
						}
					}

				}
				else{
				//	if((*i).health>0.3){
					ds=(*i).distance;
				//	_DPRINTF(("[MChenRock] attacker enemy en %f  speed %f  course %f  bear%f ",(*i).health,(*i).their_speed,(*i).their_course,(*i).bearing));
					course_changed = 0 ;
					if (best_d > ds) {
						set_course((*i).bearing);
						mycourse=(*i).bearing;
						best_d = (*i).distance;
					}
				//	}
	
				}

			}
		}
		if(best_d == HUGE){
			ST(true);
			return;
		}
		double dt(0.0);
		dt=best_d/myspeed;
		dt=12>dt?12:dt;
		hunt_event = new Event(dt, make_procedure(*this, &MChenRock::hunt));

		if (health() >= reproduceC) spawn();
	}


}
void MChenRock::ST(bool bspe){
	double ds(0.0);
	const string fav_food = "Algae";

	//_DPRINTF(("[MChenRock]Large search"));
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

	for(int i=0;i<8;i++){
		dist[i]=spedist[i]=10000.0;
	}
	int countSpe(0),count(0)  ;



	int Dit;
	for (ObjList::iterator i = prey.begin(); i != prey.end();  ++i) {

		if (bspe && (*i).species == fav_food && (*i).their_speed==0 &&!my_verify(decripted(myspeed))) { // True Algae
			countSpe++;
			double angle =normalPI((*i).bearing);
			Dit= int(angle*degree);
			spedirection[Dit]++;
			if (spedist[Dit] > (*i).distance) {
				speCR[Dit]=(*i).bearing;
				spedist[Dit] = (*i).distance;
			}
		}
		else if((*i).species!=player_name()&& !my_verify(decripted((*i).their_speed)) ){ // not myself
			count++;
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
		if(countSpe>4){
			evalution=1;
			if(drand48()<0.5){
				bAttack=false;
			}

		}
		//attackercount=0;
	//	evalution=1;
		set_course(speCR[speIndex]) ;
		mycourse=speCR[speIndex];
		double dt=(spedist[speIndex])/myspeed;
		if(dt>dTimeFre)
			dt=dTimeFre;
		//_DPRINTF(("[MChenRock]Large search max %d index:%d crouse %f",speMax,speIndex,speCR[speIndex]));
		hunt_event = new Event(10, make_procedure(*this, &MChenRock::hunt));
	}
	else if(genMax>0){   //hard time
		attackercount++;
		evalution*=0.8;
		bAttack=true;
		myspeed*=evalution;

		myspeed=encripted(myspeed);
		set_speed(myspeed);
		set_course(CR[Index]) ;
		mycourse=CR[Index];
		double dt=(dist[Index])/myspeed;
		if(dt>dTimeFre)
			dt=dTimeFre;
		//_DPRINTF(("[MChenRock]Large search Max%d index:%d dist %f",genMax,Index,CR[Index]));
		hunt_event = new Event((10+5*attackercount), make_procedure(*this, &MChenRock::hunt));
	}
	else{
		attackercount++;
		evalution*=0.6;
		bAttack=true;
		if(course_changed == 0){
			course_changed = 1 ;
			set_course(get_course() + M_PI) ;
		}
		myspeed=0;
		myspeed=encripted(myspeed);
		set_speed(myspeed);
		hunt_event = new Event((10+10*attackercount), make_procedure(*this, &MChenRock::hunt));
	}

}
void MChenRock::SetSpeed(){

	if(encounter_strategy==FASTER_GUY_WINS  ){
		myspeed=4.9+drand48()*5.0;
		myspeed=encripted(myspeed);
	}
	else if(encounter_strategy==SLOWER_GUY_WINS){
		myspeed=1.0+drand48()*3.0;
		myspeed=encripted(myspeed);
	}
	else{
		myspeed=3.0+drand48()*4.0;
		myspeed=encripted(myspeed);
	}
}

