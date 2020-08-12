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
    WaveSurface(int numWaves, double steepness, vector<double> wavelength, vector<double> amplitude, vector<double> speed, vector<Vector2D> direction);
    
  double Height(double x, double y, double time);
  double del_height_del_x(double x, double y, double time);
  double del_height_del_y(double x, double y, double time);
  double gerstner_x_sum(double x, double y, double time);
  double gerstner_y_sum(double x, double y, double time);
  double gerstner_z_sum(double x, double y, double time);
  Vector3D binormal(double x, double y, double time);
  Vector3D tangent(double x, double y, double time);
  Vector3D normal(double x, double y, double time);
  double binormal_x_component(double time, Vector2D P);
  double binormal_y_component(double time, Vector2D P);
  double binormal_z_component(double time, Vector2D P);
  double tangent_x_component(double time, Vector2D P);
  double tangent_y_component(double time, Vector2D P);
  double tangent_z_component(double time, Vector2D P);
  double normal_z_component(double time, Vector2D P);
  Vector3D gerstner_position(double x, double y, double time);



  // properties
  double Q; //steepness
  int numWaves;
  vector<Wave> waves;
  //double accu_time;
  //double delta_t;
};

#endif  //WAVESURFACE_H
