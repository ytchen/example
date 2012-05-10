#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "tokens.h"
#include "Params.h"
#include "Event.h"
#include "String.h"
#include "Window.h"
#include "ObjInfo.h"
#include "CraigUtils.h"
#include "QuadTree.h"
#include "LifeForm.h"
#include "Algae.h"

#define SPECIES_SUMMARY 1

using namespace std;
double MAX_SIMULATION_TIME = 50000.0;

/*
 * Implementation NOTE:
 * Here's a cute trick.......
 * we want to make *SURE* that the istream_creators hash table is
 * constructed *BEFORE* we try to insert anything into it....
 * But HOW can we force that to happen?  Well, instead of making
 * istream_creators a static data member, I make it a static member
 * function (returning a reference to the table).
 * the *REAL* hash table is a static local variable in this member
 * function.
 * So... when add_creator is called, it will call istream_creators()
 * and 'the_real_table' will be constructed before anything is
 * actually inserted.
 */
LFCreatorTable& LifeForm::istream_creators()
{
  static LFCreatorTable the_real_table;
  return the_real_table;
}

QuadTree<LifeForm*> LifeForm::space(0.0, 0.0, grid_max, grid_max);
Canvas LifeForm::win(win_x_size, win_y_size);

LifeForm* LifeForm::all_life = 0; // don't use Nil<> here.  
unsigned LifeForm::num_life = 0; // Nil code might be done at runtime


LifeForm::LifeForm(void)
{
  energy = start_energy;
  course = speed = 0.0;         // stationary
  pos = Point(0,0);
  is_alive = false;
  update_time = Event::now();
  reproduce_time = 0.0;
  border_cross_event = Nil<Event>();

  /* insert into all_life list */
  next_life = all_life;
  if (all_life) all_life->prev_life = this;
  prev_life = Nil<LifeForm>();
  all_life = this;
  num_life += 1;
}  


LifeForm::~LifeForm(void)
{
#if DEBUG
  cout << "death event\n";
#endif /* DEBUG */

  assert(!is_alive);
  /* remove from all_life list */
  if (next_life) next_life->prev_life = prev_life;
  if (prev_life) prev_life->next_life = next_life;
  if (all_life == this)
    all_life = next_life;
  num_life -= 1;
}


String LifeForm::player_name(void) const
{ 
  return species_name(); 
}



void LifeForm::create_life(void)
{
  LifeForm* obj;

  if (testMode) { runTests(); return; }

  String x;
  int yylex(void);
  extern FILE* yyin;
//  yyin = stdin;
  yyin = fopen("config.test", "r");
  extern String lex_text;
  extern int lex_int;
  Token tok;
  tok = (Token) yylex();
  while (tok != TOK_EOF) {
#if DEBUG
    cout << "token is " << tok << endl;
#endif /* DEBUG */
    IstreamCreator factory_fun = (istream_creators())[lex_text];
    tok = (Token) yylex();
    for (int i = 0; i < lex_int; i++) {
//      LifeForm* obj = (*k)();
      obj = factory_fun();
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
    tok = (Token) yylex();
  }
  redisplay_all();
}

void LifeForm::add_creator(IstreamCreator f, const String& s)
{
  (istream_creators())[s] = f;
}

int LifeForm::scale_x(double x) const
{
  double rel_x = (double) x / (double) grid_max;
  return (int) (rel_x * win_x_size);
}

int LifeForm::scale_y(double y) const
{
  double rel_y = (double) y / (double) grid_max;
  return (int) (rel_y * win_y_size);
}

void LifeForm::print(void) const
{
  cout << species_name() << " at ";
  print_position();
}

void LifeForm::print_position(void) const
{
  cout << "(" << pos.xpos << "," << pos.ypos << ")";
}


void LifeForm::display(void) const
{
#if DEBUG
  cout << "drawing LF at";
  cout << "(" << pos.xpos << "," << pos.ypos << ")";
#endif /* DEBUG */
  win.set_color(my_color());
  draw(scale_x(pos.xpos), scale_y(pos.ypos));
#if DEBUG
  cout << endl;
#endif /* DEBUG */
}

typedef pair<String, double> Rank;
struct RankCompare {
  bool operator()(const Rank& x, const Rank& y) {
    return x.second > y.second;
  }
};


void LifeForm::redisplay_all(void)
{
  typedef map<String, double> SpeciesHT;
  SpeciesHT species_table;
  static int max_species = 0; // the maximum number of species ever.

  win.clear();
  for (LifeForm* k = all_life; k; k = k->next_life) {
    if (k->is_alive) {
      /* uncomment the next line to get accurate graphics at the expense
         of slowing down the simulator */
      //      k->update_position();
      k->display();
      String name = k->player_name();
      name = name.substr(0, name.find(':'));
      if (species_table.find(name) == species_table.end()) {
        species_table[name] = 0.0;
      }
      species_table[name] += k->energy;
    }
  }
  win.flush();

#if (SPECIES_SUMMARY)
  cout << "\n\n\n";
  cout << "At Time " << Event::now() 
    << " there are " << num_life << " total life forms, ";
  int count = 0;
  vector<Rank> rankings;
  typedef vector<Rank>::iterator Set_IT;
  for (SpeciesHT::iterator i = species_table.begin();
       i != species_table.end();
       ++i) {
    rankings.push_back(Rank(i->first, i->second));
    count += 1;
  }
  cout << "and " << count << " distinct species\n";
  cout << "There are " << Event::num_events() << " events (" << (double) Event::num_events() 
	  / (double) num_life << " events per life form)\n";

  if (count > max_species) max_species = count;
  sort(rankings.begin(), rankings.end(), RankCompare());
  int specs = 0;

  for (vector<Rank>::iterator i = rankings.begin();
       i != rankings.end(); ++i) {
    Rank& best = *i;
    cout << "Species: " << best.first << " has total of " 
      << best.second << " energy\n";
    specs += 1;
    if (specs >= max_species / 2) {  // draw line to indicate winners/losers
      cout << "----------------------------------------";
      cout << "----------------------------------------\n";
      specs = -specs; // make sure the line is drawn only once
    }
  }

  if (termination_strategy == RUN_TILL_HALF_EXTINCT 
         && count <= max_species / 2
      || termination_strategy == RUN_TILL_ONE_SPECIES_LEFT 
         && count <= 2
	  || Event::now() > MAX_SIMULATION_TIME) {
    // abort the simulation
    cout << "\t!!Simulation Complete at time " << Event::now() << " !!\n";
    //cout << "hit CTRL-C to stop\n";  // uncomment if you want to see the
    //sleep(1000);                     // final state of the graphics display
    exit(0);
  }
#endif /* SPECIES_SUMMARY */
}

void LifeForm::draw(int x, int y) const
{
  win.draw_rectangle(x,y,x+4,y+4);
}

void LifeForm::clear_screen(void)
{
  win.clear();
}


void Algae::create_spontaneously(void)
{
  Algae* a = new Algae;
  LifeForm* nearest;
  do {
    a->pos.ypos = drand48() * grid_max / 2.0 + grid_max / 4.0;
    a->pos.xpos = drand48() * grid_max / 2.0 + grid_max / 4.0;
    nearest = space.closest(a->pos);
  } while (nearest && nearest->position().distance(a->position())
           <= encounter_distance);
  a->start_point = a->pos;

  space.insert(a, a->pos, 
               make_procedure(* static_cast<LifeForm*>(a), 
                              &LifeForm::region_resize));
  a->is_alive = true;
}


void LifeForm::die(bool in_tree) // deprecated argument
{
  if (!is_alive) return;        // already called.
                                // it is possible to call die twice in some
                                // very peculiar circumstances.
                                // you have two objects that are bumping
                                // into each other without ever eating
                                // one another.
                                // Eventually, they run low on energy.
                                // they encounter each other, and both
                                // are weak enough to die
                                // object 1 calls obj1->die();
                                // object 1 is removed from the tree.
                                // region resize is called for object 2
                                // region resize calls update position,
                                // which kills object 2 ('cause it's too weak)
                                // space.remove(obj1) returns
                                // resolve_encounter calls obj2->die();
  if (in_tree) space.remove(pos);
  is_alive = false;
  /* don't die now, but die ASAP */
  (void) new Event(0.0, make_procedure(*this, &LifeForm::_die));
}

/*
 * WARNING: THIS CODE DELETE's THIS.  USE WITH EXTREME CAUTION!
 * cancel any events pending on this LifeForm
 * delete ourself
 */
void LifeForm::_die(void)
{
  Event::delete_matching(this);
  delete this;
}
