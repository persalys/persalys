// SWIG file

%{
#include "otgui/Analysis.hxx"
%}

OTGUITypedInterfaceObjectHelper(Analysis)

%include otgui/Analysis.hxx
namespace OTGUI {

%extend Analysis { Analysis(const Analysis & other) { return new OTGUI::Analysis(other); } 

std::string __repr__() {
  return "Analysis";
}

} }
