// SWIG file

%{
#include "otgui/ParametricAnalysisResult.hxx"
%}

%include ParametricAnalysisResult_doc.i

%include otgui/ParametricAnalysisResult.hxx
namespace OTGUI {

%extend ParametricAnalysisResult { ParametricAnalysisResult(const ParametricAnalysisResult & other) { return new OTGUI::ParametricAnalysisResult(other); } 

std::string __repr__() {
  return "ParametricAnalysisResult";
}

} }
