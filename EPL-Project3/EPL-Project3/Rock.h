#if !(_Rock_h)
#define _Rock_h 1


#include "LifeForm.h"
#include "Init.h"

class Rock : public LifeForm {
  static LifeForm* create(void);
  static void initialize(void);
public:
  Rock(void);
  ~Rock(void);
  Color my_color(void) const;
  std::string species_name(void) const;
  Action encounter(const ObjInfo&);
  friend class Initializer<Rock>;
};


#endif /* !(_Rock_h) */
