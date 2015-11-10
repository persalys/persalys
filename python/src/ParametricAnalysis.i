// SWIG file

%{
#include "otgui/ParametricAnalysis.hxx"
%}



%include otgui/ParametricAnalysis.hxx
namespace OTGUI {

%extend ParametricAnalysis { ParametricAnalysis(const ParametricAnalysis & other) { return new OTGUI::ParametricAnalysis(other); } 

std::string __repr__() {
  return "ParametricAnalysis";
}

} }
