// SWIG file

%{
#include "otgui/LimitStateImplementation.hxx"
%}

%include LimitStateImplementation_doc.i

%include otgui/LimitStateImplementation.hxx
namespace OTGUI {

%extend LimitStateImplementation { LimitStateImplementation(const LimitStateImplementation & other) { return new OTGUI::LimitStateImplementation(other); } 

std::string __repr__() {
  return "LimitStateImplementation";
}

} }
