
#include <fstream>
#include <iomanip>
#include <iostream>

#include "camera.h"
#include "constants.h"
#include "hittable_list.h"
#include "materials/dielectric.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "sphere.h"

vec3 ray_color(const ray& r, const hittable& world, int depth);
double hit_sphere(const vec3& center, double radius, const ray& r);
hittable_list random_scene();

int main() {
  const int image_width = getenv("WIDTH") == NULL ? 640 : atoi(getenv("WIDTH"));
  const int image_height =
      getenv("HEIGHT") == NULL ? 480 : atoi(getenv("HEIGHT"));
  const auto aspect_ratio = double(image_width) / image_height;
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  vec3 lookfrom(0, 5, 5);
  vec3 lookat(0, 0, -1);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = (lookfrom - lookat).length();
  auto aperture = 0.05;

  auto out_file_path = getenv("OUT_FILE_PATH");
  std::cout << "Opening " << out_file_path << "\n";

  std::ofstream out_file;
  out_file.open(out_file_path);

  // Write ppm file header
  out_file << "P3\n" << image_width << " " << image_height << "\n255\n";

  auto world = random_scene();

  camera cam(lookfrom, lookat, vup, 90, aspect_ratio, aperture, dist_to_focus);

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

hittable_list random_scene() {
  hittable_list world;

  world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

  int i = 1;
  for (int a = -5; a < 5; a++) {
    for (int b = -5; b < 5; b++) {
      auto choose_mat = random_double();
      vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = vec3::random() * vec3::random();
          world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<lambertian>(albedo)));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = vec3::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<metal>(albedo, fuzz)));
        } else {
          // glass
          world.add(
              make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
        }
      }
    }
  }

  world.add(
      make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

  world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0,
                                make_shared<lambertian>(vec3(0.4, 0.2, 0.1))));

  world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0,
                                make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

  return world;
}