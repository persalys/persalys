// SWIG file

%{
#include "QuadraticCumulCalculus.hxx"
%}



%include QuadraticCumulCalculus.hxx
namespace OTGUI {

%extend QuadraticCumulCalculus { QuadraticCumulCalculus(const QuadraticCumulCalculus & other) { return new OTGUI::QuadraticCumulCalculus(other); } 

std::string __repr__() {
  return "QuadraticCumulCalculus";
}

} }
