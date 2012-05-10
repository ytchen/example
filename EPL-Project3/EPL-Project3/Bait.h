#if !(_Bait_h)
#define _Bait_h 1


#include "LifeForm.h"
#include "Init.h"

class Bait : public LifeForm {
  static LifeForm* create(void);
  static void initialize(void);
  void birth(void);
  void wake_up(void);
  virtual void choose_course(ObjList&);
  void hibernate(void);
  virtual void fission(void);
public:
  Bait(void);
  ~Bait(void);
  Color my_color(void) const;
  std::string species_name(void) const;
  std::string player_name(void) const;
  Action encounter(const ObjInfo&);
  friend class Initializer<Bait>;
};

class ShyBait : public Bait {
public:
  virtual void choose_course(ObjList&);
  virtual void fission(void);
};

#endif /* !(_Bait_h) */
