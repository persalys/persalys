// SWIG file

%{
#include "OTStudy.hxx"
%}


%include OTStudy.hxx
namespace OTGUI {

%extend OTStudy { OTStudy(const OTStudy & other) { return new OTGUI::OTStudy(other); } 

std::string __repr__() {
  return "OTStudy";
}

} }
