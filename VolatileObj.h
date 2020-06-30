/**
 * @file VolatileObj.h
 *
 * Defines an "volatile" object that can explode.
 */

#ifndef VOLATILE_OBJ_H_2020_06_28
#define VOLATILE_OBJ_H_2020_06_28

#include "GraphObj.h"

class VolatileObj : public GraphObj
{
public:
  VolatileObj() {}

  virtual std::list<std::shared_ptr<GraphObj>> explode();

  void setExplosionRatio(float ratio) { mExplosionRatio = ratio; }
  float getExplosionRatio() { return mExplosionRatio; }

protected:
  enum class ThrowStyle 
  {
    Explosion,
    Breakup
  };
  void throwObjRand(std::shared_ptr<GraphObj> obj, ThrowStyle throwStyle = ThrowStyle::Explosion);

  enum class ExplodeStyle
  {
    FireAndFragments,
    FireOnly
  };
  ExplodeStyle mExplodeStyle = ExplodeStyle::FireAndFragments;

  float mExplosionRatio = 1; // Larger number for more explosion effects
};

#endif