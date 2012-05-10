#include <iostream>
#include "Mem.h"
#include "LifeForm.h"
#include "Algae.h"
#include "Event.h"
#include "Params.h"

using namespace std;

const double Point::tolerance = 1.0e-6;

class Tick {
public:
  static void tock(void) {
    Procedure0 callme = make_procedure(tock);
#if ALGAE_SPORES    
    Algae::create_spontaneously();
#endif /* ALGAE_SPORES */
    if (Event::num_events() > 1)
      (void) new Event(1, callme);
  }
};

int main(int argc, char** argv) {
  double last_time = 0.0;
  double time_lapse;
//  struct timeval clock_time;

  //  gettimeofday(&clock_time, 0);
  //cout << "Random seed is " << clock_time.tv_usec << endl;
  //srand48(clock_time.tv_usec);  // set random seed
//  srand48(645897);
  srand(4242);
  
  if (argc > 1)
    time_lapse = atof(argv[1]);
  else
    time_lapse = 1.0;
  
  LifeForm::create_life();
  Tick::tock();
  while (Event::num_events() > 0) {
    Event::do_next();
    /* periodically redisplay everything */
    if (Event::now() - last_time > time_lapse) {
      last_time = Event::now();
      LifeForm::redisplay_all();
    }
  }
  
  cerr << "Simulation Complete, hit ^C to terminate program\n";
  //  sleep(1000);
}
