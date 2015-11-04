// SWIG file

%{
#include "otgui/SobolCalculus.hxx"
%}


%include otgui/SobolCalculus.hxx
namespace OTGUI {

%extend SobolCalculus { SobolCalculus(const SobolCalculus & other) { return new OTGUI::SobolCalculus(other); } 

std::string __repr__() {
  return "SobolCalculus";
}

} }
