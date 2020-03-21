#include "camera.h"
#include "float.h"
#include "hittable_list.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "random.h"
#include "sphere.h"

#include <fstream>
#include <iomanip>
#include <iostream>

vec3 color(const ray& r, hittable* world, int depth);
float hit_sphere(const vec3& center, float radius, const ray& r);

int main() {
  int nx = 700;
  int ny = 350;
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

  hittable* list[4];
  list[0] =
      new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
  list[1] =
      new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
  list[2] =
      new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.1));
  list[3] =
      new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 1.0));
  hittable* world = new hittable_list(list, 4);
  camera cam;

  const int total_iterations = nx * ny;
  int current_iterations = 0;

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + random_double()) / float(nx);
        float v = float(j + random_double()) / float(ny);
        ray r = cam.get_ray(u, v);
        col += color(r, world, 0);
      }
      col /= float(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

      int ir = int(255.99 * col.r());
      int ig = int(255.99 * col.g());
      int ib = int(255.99 * col.b());

      out_file << ir << " " << ig << " " << ib << "\n";

      // Progress indicator, this takes a while...
      std::cout << std::setw(3)
                << int(current_iterations++ / float(total_iterations) * 100)
                << "%"
                << "\r";
    }
  }
  std::cout << "\n";

  out_file.close();

  return 0;
}

vec3 color(const ray& r, hittable* world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.001, MAXFLOAT, rec)) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(scattered, world, depth + 1);
    } else {
      return vec3(0, 0, 0);
    }
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
