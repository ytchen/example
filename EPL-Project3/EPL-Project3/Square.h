#if !(_Square_h)
#define _Square_h 1


#include "LifeForm.h"
#include "Init.h"

class Square : public LifeForm {
  static LifeForm* create(void);
  static void initialize(void);
  void live(void);
  void turn(void);
  Event* turn_event;
public:
  Square(void);
  ~Square(void);
  Color my_color(void) const;
  std::string species_name(void) const;
  Action encounter(const ObjInfo&);
  friend class Initializer<Square>;
};


#endif /* !(_Square_h) */
