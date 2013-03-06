#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;

protected:
  Material() {}
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;

  const Colour & get_diffuse()   const { return m_kd; }
  const Colour & get_specular()  const { return m_ks; }
  double         get_shininess() const { return m_shininess; }

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
