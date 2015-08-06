// SWIG file

%{
#include "Observer.hxx"
%}


%include Observer.hxx
namespace OTGUI {

%extend Observer { Observer(const Observer & other) { return new OTGUI::Observer(other); } 

std::string __repr__() {
  return "Observer";
}

} }
