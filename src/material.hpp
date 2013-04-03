#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Material Base

class Material {
public:
  virtual ~Material() {}
};

///////////////////////////////////////////////////////////////////////////////
// Phong Material

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double reflectivity = 0)
      : m_kd(kd), m_ks(ks), m_shininess(shininess), m_refl(reflectivity) {}

  const Colour & get_diffuse()      const { return m_kd; }
  const Colour & get_specular()     const { return m_ks; }
  double         get_shininess()    const { return m_shininess; }
  double         get_reflectivity() const { return m_refl; }

private:
  Colour m_kd;
  Colour m_ks;
  double m_shininess;
  double m_refl;
};

///////////////////////////////////////////////////////////////////////////////
// Optics Material

class OpticsMaterial : public Material {
public:
  OpticsMaterial(double index, double opacity = 0, const Colour & ko = Colour(0.9,0.9,0.9))
      : m_index(index), m_opacity(opacity), m_ko(ko) {}

  double         get_index()   const { return m_index; }
  double         get_opacity() const { return m_opacity; }
  const Colour & get_colour()  const { return m_ko; }

private:
  double m_index;
  double m_opacity;
  Colour m_ko;
};

///////////////////////////////////////////////////////////////////////////////
// Texture Material

class TextureMaterial : public Material {
public:
  TextureMaterial(const char * file, const Colour & ks, double shininess)
      : m_file(file), m_ks(ks), m_shininess(shininess) {}

  const std::string & get_texture_file() const { return m_file; }
  const Colour & get_specular()  const { return m_ks; }
  double         get_shininess() const { return m_shininess; }

private:
  std::string m_file;
  Colour m_ks;
  double m_shininess;
};

#endif

