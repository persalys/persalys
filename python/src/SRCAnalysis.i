// SWIG file

%{
#include "otgui/SRCAnalysis.hxx"
%}


%include otgui/SRCAnalysis.hxx
namespace OTGUI {

%extend SRCAnalysis { SRCAnalysis(const SRCAnalysis & other) { return new OTGUI::SRCAnalysis(other); } 

std::string __repr__() {
  return "SRCAnalysis";
}

} }
