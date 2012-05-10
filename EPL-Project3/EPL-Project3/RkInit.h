#if !(_Rk_Init_h)
#define _Rk_Init_h 1

#include <string>
#include "LifeForm.h"
using namespace std;
template <class T>
class RkInitializer
{
public:

   RkInitializer(string yyName)
   {
      static bool initialized = false;
      if (! initialized)
      {
         initialized = true;
         LifeForm::add_creator(T::create, yyName);
      }
   }

};



#endif /* !(_Rk_Init_h) */
