// SWIG file

%{
#include "otgui/QuadraticCumulCalculus.hxx"
%}



%include otgui/QuadraticCumulCalculus.hxx
namespace OTGUI {

%extend QuadraticCumulCalculus { QuadraticCumulCalculus(const QuadraticCumulCalculus & other) { return new OTGUI::QuadraticCumulCalculus(other); } 

std::string __repr__() {
  return "QuadraticCumulCalculus";
}

} }
