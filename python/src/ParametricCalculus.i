// SWIG file

%{
#include "otgui/ParametricCalculus.hxx"
%}



%include otgui/ParametricCalculus.hxx
namespace OTGUI {

%extend ParametricCalculus { ParametricCalculus(const ParametricCalculus & other) { return new OTGUI::ParametricCalculus(other); } 

std::string __repr__() {
  return "ParametricCalculus";
}

} }
