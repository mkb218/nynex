#include "XmlParser.hpp"

auto_ptr<nynex::fs1rgen::Model> nynex::fs1rgen::XmlParser::getModel()  {
  return auto_ptr<Model>(new Model(m_model));
}
