#ifndef WAVESURFACE_H
#define WAVESURFACE_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "wave.h"

using namespace CGL;
using namespace std;

struct WaveSurface {
  WaveSurface() {}
  WaveSurface(double numWaves, double steepness);
    
  double Height(double x, double y, double time);
  double del_height_del_x(double x, double y, double time);
  double del_height_del_y(double x, double y, double time);
  double gerstner_x_sum(double x, double y, double time);
  double gerstner_y_sum(double x, double y, double time);
  double gerstner_z_sum(double x, double y, double time);
  double binormal_x_component(double S);
  double binormal_y_component(double S);
  double binormal_z_component(double C);
  double tangent_x_component(double S);
  double tangent_y_component(double S);
  double tangent_z_component(double C);
  double normal_z_component(double S);
  Vector3D gerstner_position(double x, double y, double time);



  // properties
  double Q; //steepness
  double numWaves;
  vector<Wave> waves;
  //double accu_time;
  //double delta_t;
};

#endif  //WAVESURFACE_H
