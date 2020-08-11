#ifndef WAVE_H
#define WAVE_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector3D.h"
#include "CGL/vector2D.h"

//#include "collision/collisionObject.h"
//#include "spring.h"

using namespace CGL;
using namespace std;

struct Wave {
  Wave(){};
    
  Wave(double wavelength, double amplitude, double speed, Vector2D direction);

  double gerstner_pos_x(double x, double y, double time, double qi);

  double gerstner_pos_y(double x, double y, double time, double qi);

  double gerstner_pos_z(double x, double y, double time, double qi);
    
  Vector3D gerstner_position(double x, double y, double time);
    
  Vector3D binormal(double x, double y, double time);
    
  Vector3D tangent(double x, double y, double time);
    
  Vector3D normal(double x, double y, double time);

  // Cloth properties
  double wavelength;
  double amplitude;
  double speed;
  Vector2D direction;
  double frequency;
  double WA;
  double phi;
  
  //WaveSurface wavesurface;
};

#endif /* WAVE_H */
