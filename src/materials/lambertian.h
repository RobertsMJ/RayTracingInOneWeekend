#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H
#include "../constants.h"
#include "material.h"

class lambertian : public material {
 public:
  lambertian(const vec3& a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       vec3& attenuation, ray& scattered) const {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

  vec3 albedo;
};

#endif