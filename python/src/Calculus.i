// SWIG file

%{
#include "Calculus.hxx"
%}



%include Calculus.hxx
namespace OTGUI {

%extend Calculus { Calculus(const Calculus & other) { return new OTGUI::Calculus(other); } 

std::string __repr__() {
  return "Calculus";
}

} }
