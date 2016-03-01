// SWIG file Analysis.i

%{
#include "otgui/Analysis.hxx"
%}

%include Analysis_doc.i

OTGUITypedInterfaceObjectHelper(Analysis)

%include otgui/Analysis.hxx
namespace OTGUI {

%extend Analysis { Analysis(const Analysis & other) { return new OTGUI::Analysis(other); } 

std::string __repr__() {
  return "Analysis";
}

} }
