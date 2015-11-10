// SWIG file

%{
#include "otgui/AnalysisImplementation.hxx"
%}



%include otgui/AnalysisImplementation.hxx
namespace OTGUI {

%extend AnalysisImplementation { AnalysisImplementation(const AnalysisImplementation & other) { return new OTGUI::AnalysisImplementation(other); } 

std::string __repr__() {
  return "AnalysisImplementation";
}

} }
