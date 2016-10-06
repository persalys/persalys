// SWIG file

%{
#include "otgui/MetaModelAnalysisResult.hxx"
%}

%include MetaModelAnalysisResult_doc.i

%include otgui/MetaModelAnalysisResult.hxx
namespace OTGUI {

%extend MetaModelAnalysisResult { MetaModelAnalysisResult(const MetaModelAnalysisResult & other) { return new OTGUI::MetaModelAnalysisResult(other); } 

std::string __repr__() {
  return "MetaModelAnalysisResult";
}

} }
