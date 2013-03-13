#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include <string>

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

class OpticsMaterial : public Material {
public:
  OpticsMaterial(double index) : m_index(index) {}
  virtual ~OpticsMaterial() {}

  double get_index() const { return m_index; }
  virtual void apply_gl() const {}

private:
  double m_index;
};

class TextureMaterial : public Material {
public:
  TextureMaterial(const char * file, const Colour & ks, double shininess)
      : m_file(file), m_ks(ks), m_shininess(shininess) {}
  virtual ~TextureMaterial() {}

  const std::string & get_texture_file() const { return m_file; }
  const Colour & get_specular()  const { return m_ks; }
  double         get_shininess() const { return m_shininess; }

  virtual void apply_gl() const {}

private:
  std::string m_file;
  Colour m_ks;
  double m_shininess;
};

#endif
