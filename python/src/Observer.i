// SWIG file

%{
#include "otgui/Observer.hxx"
%}


%include otgui/Observer.hxx
namespace OTGUI {

%extend Observer { Observer(const Observer & other) { return new OTGUI::Observer(other); } 

std::string __repr__() {
  return "Observer";
}

} }
