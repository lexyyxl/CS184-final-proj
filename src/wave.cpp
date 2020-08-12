#define _USE_MATH_DEFINES

#include <cmath>

#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "wave.h"
#include "CGL/vector3D.h"
#include "CGL/vector2D.h"
#include "cloth.h"

using namespace std;

double POWER_CONSTANT = 1.0;
double g = 9.8;




Wave::Wave(double wavelength, double amplitude, double speed, Vector2D direction) {
    this->wavelength = wavelength;
    //this->wavelength = sqrt(g* 2* M_PI/wavelength);
    this->amplitude = amplitude;
    this->speed = speed;
    this->direction = direction;
    //this->waveSurface = wavesurface;
    this->frequency = 2 / wavelength; //w_i
    this->WA = (2 / wavelength) * amplitude;
    //phase_constant
    this->phi = speed * 2 / wavelength;
    
}

double Wave::gerstner_pos_x(double x, double y, double time, double qi) {
    return qi * amplitude * direction.x * cos(dot(frequency * direction, Vector2D(x, y)) + phi * time);
}

double Wave::gerstner_pos_y(double x, double y, double time, double qi) {
    return qi * amplitude * direction.y * cos(dot(frequency * direction, Vector2D(x, y)) + phi * time);
}

double Wave::gerstner_pos_z(double x, double y, double time, double qi) {
    return amplitude * sin(dot(frequency * direction, Vector2D(x, y)) + phi * time);
}



