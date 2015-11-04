// SWIG file

%{
#include "otgui/SRCCalculus.hxx"
%}


%include otgui/SRCCalculus.hxx
namespace OTGUI {

%extend SRCCalculus { SRCCalculus(const SRCCalculus & other) { return new OTGUI::SRCCalculus(other); } 

std::string __repr__() {
  return "SRCCalculus";
}

} }
