// SWIG file

%{
#include "otgui/Calculus.hxx"
%}

OTGUITypedInterfaceObjectHelper(Calculus)

%include otgui/Calculus.hxx
namespace OTGUI {

%extend Calculus { Calculus(const Calculus & other) { return new OTGUI::Calculus(other); } 

std::string __repr__() {
  return "Calculus";
}

} }
