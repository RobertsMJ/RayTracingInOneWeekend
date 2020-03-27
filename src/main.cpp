
#include "constants.h"

#include "camera.h"
#include "hittable_list.h"
#include "materials/dielectric.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "sphere.h"

#include <fstream>
#include <iomanip>
#include <iostream>

vec3 ray_color(const ray& r, const hittable& world, int depth);
double hit_sphere(const vec3& center, double radius, const ray& r);

int main() {
  const int image_width = 640;
  const int image_height = 480;
  const auto aspect_ratio = double(image_width) / image_height;
  const int samples_per_pixel = 100;
  const int max_depth = 50;
  const vec3 vup(0, 1, 0);

  auto out_file_path = getenv("OUT_FILE_PATH");
  std::cout << "Opening " << out_file_path << "\n";

  std::ofstream out_file;
  out_file.open(out_file_path);

  // Write ppm file header
  out_file << "P3\n" << image_width << " " << image_height << "\n255\n";

  hittable_list world;
  world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5,
                                make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));
  world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100,
                                make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));
  world.add(make_shared<sphere>(vec3(1, 0, -1), 0.5,
                                make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.3)));
  world.add(
      make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(vec3(-1, 0, -1), -0.45,
                                make_shared<dielectric>(1.5)));

  camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vup, 90, aspect_ratio);

  for (int j = image_height - 1; j >= 0; j--) {
    for (int i = 0; i < image_width; ++i) {
      vec3 color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / image_width;
        auto v = (j + random_double()) / image_height;
        ray r = cam.get_ray(u, v);
        color += ray_color(r, world, max_depth);
      }
      color.write_color(out_file, samples_per_pixel);
      // Progress indicator, this takes a while...
      std::cout << std::setw(5)
                << int(((image_height - j) / double(image_height)) * 100) << "%"
                << "\r";
    }
  }
  std::cout << "\n";

  out_file.close();

  return 0;
}

vec3 ray_color(const ray& r, const hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) return vec3(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec)) {
    ray scattered;
    vec3 attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return vec3(0, 0, 0);
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
