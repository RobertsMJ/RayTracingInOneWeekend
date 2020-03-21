#include "camera.h"
#include "float.h"
#include "hittable_list.h"
#include "random.h"
#include "sphere.h"

#include <fstream>
#include <iostream>

vec3 color(const ray& r, hittable* world);
float hit_sphere(const vec3& center, float radius, const ray& r);

int main() {
  int nx = 200;
  int ny = 100;
  int ns = 100;

  auto out_file_path = getenv("OUT_FILE_PATH");
  std::cout << "Opening " << out_file_path << "\n";

  std::ofstream out_file;
  out_file.open(out_file_path);

  // Write ppm file header
  out_file << "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);

  hittable* list[2];
  list[0] = new sphere(vec3(0, 0, -1), 0.5);
  list[1] = new sphere(vec3(0, -100.5, -1), 100);
  hittable* world = new hittable_list(list, 2);
  camera cam;

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + random_double()) / float(nx);
        float v = float(j + random_double()) / float(ny);
        ray r = cam.get_ray(u, v);
        col += color(r, world);
      }
      col /= float(ns);

      int ir = int(255.99 * col.r());
      int ig = int(255.99 * col.g());
      int ib = int(255.99 * col.b());

      out_file << ir << " " << ig << " " << ib << "\n";
    }
  }

  out_file.close();

  return 0;
}

vec3 color(const ray& r, hittable* world) {
  hit_record rec;
  if (world->hit(r, 0.0, MAXFLOAT, rec)) {
    return 0.5 *
           vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

// Detect ray intersections with a sphere
float hit_sphere(const vec3& center, float radius, const ray& r) {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0 * dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-b - sqrt(discriminant)) / (2.0 * a);
  }
}
