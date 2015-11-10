// SWIG file

%{
#include "otgui/SobolAnalysis.hxx"
%}


%include otgui/SobolAnalysis.hxx
namespace OTGUI {

%extend SobolAnalysis { SobolAnalysis(const SobolAnalysis & other) { return new OTGUI::SobolAnalysis(other); } 

std::string __repr__() {
  return "SobolAnalysis";
}

} }
