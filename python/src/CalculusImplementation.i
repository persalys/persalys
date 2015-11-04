// SWIG file

%{
#include "otgui/CalculusImplementation.hxx"
%}



%include otgui/CalculusImplementation.hxx
namespace OTGUI {

%extend CalculusImplementation { CalculusImplementation(const CalculusImplementation & other) { return new OTGUI::CalculusImplementation(other); } 

std::string __repr__() {
  return "CalculusImplementation";
}

} }
