// SWIG file

%{
#include "otgui/LimitState.hxx"
%}

%include LimitState_doc.i

OTGUITypedInterfaceObjectHelper(LimitState)

%include otgui/LimitState.hxx
namespace OTGUI {

%extend LimitState { LimitState(const LimitState & other) { return new OTGUI::LimitState(other); } 

std::string __repr__() {
  return "LimitState";
}

} }
