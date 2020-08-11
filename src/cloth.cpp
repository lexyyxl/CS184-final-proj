#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"
#include "waveSurface.h"
#include "CGL/vector3D.h"
#include "CGL/vector2D.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.

    double dw = (double)this->width/(this->num_width_points - 1);
    double dh = (double)this->height/(this->num_height_points - 1);

    int num_waves = 4;
    
    //double numWaves, double steepness
    this->wavesurface = WaveSurface(num_waves, 0.03);
    
    for (int i = 0; i < this->num_height_points; ++i) {
        for (int j = 0; j < this->num_width_points; ++j) {
            Vector3D pos;
            double pw = j * dw;
            double ph = i * dh;
            if (this->orientation == HORIZONTAL) {
                pos = Vector3D(pw, 1.0, ph);
            } else {
                pos = Vector3D(pw, ph, (rand()/(double)RAND_MAX * 2.0 - 1.0) /1000.0);
            }
            bool p_pinned = false;
            for (int k = 0; k < pinned.size(); k++) {
                if ((pinned[k][0] == j) && (pinned[k][1] == i)) {
                    p_pinned = true;
                    break;
                }
            }
            this->point_masses.emplace_back(PointMass(pos, p_pinned));
        }
    }

    //Structural
    for (int i = 0; i < this->num_height_points; ++i) { // row ind
        for (int j = 1; j < this->num_width_points; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_left = pm - 1;
            this->springs.emplace_back(Spring(pm, pm_left, STRUCTURAL));
        }
    }


    for (int i = 1; i < this->num_height_points; ++i) { // row ind
        for (int j = 0; j < this->num_width_points; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_up = pm - num_width_points;
            this->springs.emplace_back(Spring(pm, pm_up, STRUCTURAL));
        }
    }

    //Shearing

    for (int i = 1; i < this->num_height_points; ++i) { // row ind
        for (int j = 1; j < this->num_width_points; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_up_left = pm - num_width_points - 1;
            this->springs.emplace_back(Spring(pm, pm_up_left, SHEARING));
        }
    }

    for (int i = 1; i < this->num_height_points; ++i) { // row ind
        for (int j = 0; j < this->num_width_points - 1; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_up_right = pm - num_width_points + 1;
            this->springs.emplace_back(Spring(pm, pm_up_right, SHEARING));
        }
    }

    //bending

    for (int i = 0; i < this->num_height_points; ++i) { // row ind
        for (int j = 2; j < this->num_width_points; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_left_2 = pm - 2;
            this->springs.emplace_back(Spring(pm, pm_left_2, BENDING));
        }
    }

    for (int i = 2; i < this->num_height_points; ++i) { // row ind
        for (int j = 0; j < this->num_width_points; ++j) { //col ind
            PointMass* pm = &(this->point_masses[i * num_width_points + j]);
            PointMass* pm_up_2 = pm - 2 * num_width_points;
            this->springs.emplace_back(Spring(pm, pm_up_2, BENDING));
        }
    }
}



void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {

  //  cout << cp->density <<endl;
  //cp->density = 1000;
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;


  // TODO (Part 2): Compute total force acting on each point mass.


    Vector3D total_external_f = Vector3D(0, 0, 0);
    for (int i = 0; i < external_accelerations.size(); ++i) {
        total_external_f += external_accelerations[i] * (double)mass;
    }
    for (int i = 0; i < point_masses.size(); ++i) {
        point_masses[i].forces = total_external_f;
    }


    for (int i = 0; i < springs.size(); ++i) {

        bool enabled = false;
        Spring s = springs[i];

        switch (s.spring_type) {
            case CGL::BENDING:
                enabled = cp->enable_bending_constraints;
            case CGL::SHEARING:
                enabled = cp->enable_shearing_constraints;
            default:
                enabled = cp->enable_structural_constraints;
        }

        double F_s_mag;
        if (enabled) {
            //cout<<cp->ks<<endl;
            //cp->ks = 50000;
            F_s_mag = (double)cp->ks * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);

            if (s.spring_type == BENDING) {
                F_s_mag *= 0.2;
            }

            Vector3D dir_ab = (s.pm_b->position - s.pm_a->position).unit();
            s.pm_a->forces += F_s_mag * dir_ab;
            s.pm_b->forces -= F_s_mag * dir_ab;
        }
    }


  // TODO (Part 2): Use Verlet integration to compute new point mass positions

    for (int i = 0; i < point_masses.size(); ++i) {
        //cout << cp->damping <<endl;
        //cp->damping = 0.999;
        PointMass* pm = &point_masses[i];
        if (!pm->pinned) {
            
            Vector3D new_pos = this->wavesurface.gerstner_position(pm->position.x, pm->position.y, time);
                //+ 
                //(1 - cp->damping / 100) * (pm->position - pm->last_position) + pm->forces / mass * delta_t * delta_t;
            
            //pm->position + (1 - cp->damping / 100) * (pm->position - pm->last_position) + pm->forces / mass * delta_t * delta_t;

            pm->last_position = Vector3D(pm->position.x, pm->position.y, pm->position.z);
            pm->position = new_pos;
        }
    }

    time += delta_t;


  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
    for (auto s: springs) {
        PointMass* a = s.pm_a;
        PointMass* b = s.pm_b;
        Vector3D dir_ab = (b->position - a->position).unit();
        Vector3D mid = (b->position + a->position) / 2.0;
        int num_constrained_pm = 2 - a->pinned - b->pinned;

        if (num_constrained_pm <= 0) {
            //cout << num_constrained_pm <<endl;
            continue;
        }

        if ((b->position - a->position).norm() > 1.1 * s.rest_length) {
            if (a->pinned) {
                b->position = a->position + dir_ab * 1.1 * s.rest_length;
            } else if (b->pinned) {
                a->position = b->position - dir_ab * 1.1 * s.rest_length;
            } else {
                a->position = mid - dir_ab * 0.55 * s.rest_length;
                b->position = mid + dir_ab * 0.55 * s.rest_length;
            }
        }
    }
}


void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.

    for (PointMass &pm : point_masses) {
        double key = hash_position(pm.position);
        if (map.count(key) == 0 ) { //does not contain k
            map[key] = new vector<PointMass*>();
        }
        map[key] -> push_back(&pm);

//        for (PointMass* c: *map[key]) {
//            cout << c->position <<endl;
//        }



    }
}


void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.

    //build_spatial_map();
    
    Vector3D correction = Vector3D(0.0, 0.0, 0.0);
    int count = 0;
    double key = hash_position(pm.position);

    for (PointMass* candidate : *map[key]) {
        if (candidate != &pm){
            Vector3D vec = pm.position - candidate->position;
            if (vec.norm() < 2 * thickness) {
                Vector3D ideal_pos = candidate->position + vec.unit() * 2 * thickness;
                correction += ideal_pos - pm.position;
                count += 1;
            }
        }
    }
    if (count > 0) {
        pm.position += correction / count / simulation_steps;
    }
}


float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
    double w = 3 * width / num_width_points;
    double h = 3 * height / num_height_points;
    double t = max(w, h);
    int x = floor(pos.x / t);
    int y = floor(pos.y / t);
    int z = floor(pos.z / t);
    return 51 * 51 * pow(2, x) + 51 * pow(2, y) + pow(2, z);
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */

      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;

      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;

      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);


      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B,
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D,
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}




