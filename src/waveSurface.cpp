#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "wave.h"
#include "waveSurface.h"

using namespace std;
const double phi = 1.;
const double POWER_CONSTANT = 2.;

WaveSurface::WaveSurface(int numWaves, double steepness, vector<double> wavelength, vector<double> amplitude, vector<double> speed, vector<Vector2D> direction  ) {
    this->numWaves = numWaves;
    this->Q = steepness;
    for (int i = 0; i < numWaves; ++i) {
        Wave w = Wave(wavelength[i], amplitude[i], speed[i], direction[i]);
        this->waves.emplace_back(w);
    }
}


Vector3D WaveSurface::gerstner_position(double x, double y, double time) {
    //this->accu_time += delta_t;
    return Vector3D(x + gerstner_x_sum(x, y, time),
        y + gerstner_y_sum(x, y, time),
        gerstner_z_sum(x, y, time));
}

//gerstner wave binormal vector
Vector3D WaveSurface::binormal(double x, double y, double time) {
    Vector2D P = Vector2D(x, y);
    return Vector3D(this->binormal_x_component(time, P),
        this->binormal_y_component(time, P),
        this->binormal_z_component(time, P));
}

//gerstner wave tangent vector
Vector3D WaveSurface::tangent(double x, double y, double time) {
    Vector2D P = Vector2D(x, y);
    return Vector3D(this->tangent_x_component(time, P),
        this->tangent_y_component(time, P),
        this->tangent_z_component(time, P));
}

//gerstner wave normal vector
//Vector3D WaveSurface::normal(double x, double y, double time, Wave w) {
//    Vector2D P = Vector2D(x, y);
//    double S = sin(w.frequency * dot(w.direction, P) + phi * time);
//    double C = cos(w.frequency * dot(w.direction, P) + phi * time);
//    return Vector3D(-this->normal_z_component(C),
//        -this->tangent_z_component(C),
//        this->normal_z_component(S));
//}

Vector3D WaveSurface::normal(double x, double y, double time) {
    Vector2D P = Vector2D(x, y);  
    return Vector3D(-this->binormal_z_component(time, P),
        -this->tangent_z_component(time, P),
        this->normal_z_component(time, P));
}


//double WaveSurface::Height(double x, double y, double time) {
//    double sum = 0.0;
//    for (Wave i : waves) {
//        sum += gerstner_position(x, y, time);
//    }
//    return sum;
//}

double WaveSurface::del_height_del_x(double x, double y, double time) {
    double sum = 0.0;
    for (Wave w : waves) {
        sum += POWER_CONSTANT * w.frequency * w.direction.x * w.amplitude * cos(dot(w.direction, Vector2D(x, y) * w.frequency) + time * phi) * pow((sin(dot(w.direction, Vector2D(x, y) * w.frequency) + time * phi) + 1) / 2, POWER_CONSTANT - 1);
    }
    return sum;
}


double WaveSurface::del_height_del_y(double x, double y, double time) {
    double sum = 0.0;
    for (Wave w : waves) {
        sum += w.frequency * w.direction.y * w.amplitude * cos(dot(w.direction, Vector2D(x, y) * w.frequency) + time * phi);
    }
    return sum;
}

double WaveSurface::gerstner_x_sum(double x, double y, double time) {
    double sum = 0.0;
    for (Wave w : waves) {
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += w.gerstner_pos_x(x, y, time, qi);
    }
    return sum;
}

double WaveSurface::gerstner_y_sum(double x, double y, double time) {
    double sum = 0.0;
    for (Wave w : waves) {
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += w.gerstner_pos_y(x, y, time, qi);
    }
    return sum;
}

double WaveSurface::gerstner_z_sum(double x, double y, double time) {
    double sum = 0.0;
    for (Wave w : waves) {
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += w.gerstner_pos_z(x, y, time, qi);
    }
    return sum;
}
//gerstner wave binormal x component (x, y, t)
double WaveSurface::binormal_x_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double S = sin(w.frequency * dot(w.direction, P) + phi * time);
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += qi * pow(w.direction.x, 2) * w.WA * S;
    }
    return 1 - sum;
}



//gerstner wave binormal y component for (x, y, t)
double WaveSurface::binormal_y_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double S = sin(w.frequency * dot(w.direction, P) + phi * time);
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += qi * w.direction.x * w.direction.y * w.WA * S;
    }
    return -sum;
}


//gerstner wave binormal z component (x, y, t) or negative normal x component
double WaveSurface::binormal_z_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double C = cos(w.frequency * dot(w.direction, P) + phi * time);
        sum += w.direction.x * w.WA * C;
    }
    return sum;
}

//gerstner wave tangent x component (x, y, t)
double WaveSurface::tangent_x_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double S = sin(w.frequency * dot(w.direction, P) + phi * time);
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += qi * w.direction.x * w.direction.y * w.WA * S;
    }
    return -sum;
}

//gerstner wave tangent y component (x, y, t)
double WaveSurface::tangent_y_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double S = sin(w.frequency * dot(w.direction, P) + phi * time);
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += qi * pow(w.direction.y, 2) * w.WA * S;
    }
    return 1 - sum;
}

//gerstner wave tangent z component (x, y, t) or negative normal y component
double WaveSurface::tangent_z_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double C = cos(w.frequency * dot(w.direction, P) + phi * time);
        sum += w.direction.y * w.WA * C;
    }
    return sum;
}

//gerstner wave normal z component (x, y, t)
double WaveSurface::normal_z_component(double time, Vector2D P) {
    double sum = 0.0;
    for (Wave w : waves) {
        double S = sin(w.frequency * dot(w.direction, P) + phi * time);
        double qi = Q / (w.frequency * w.amplitude * numWaves);
        sum += qi * w.WA * S;
    }
    return 1 - sum;
}
