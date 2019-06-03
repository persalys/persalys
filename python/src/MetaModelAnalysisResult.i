// SWIG file

%{
#include "persalys/MetaModelAnalysisResult.hxx"
%}

%include MetaModelAnalysisResult_doc.i

%include persalys/MetaModelAnalysisResult.hxx
namespace PERSALYS {

%extend MetaModelAnalysisResult { MetaModelAnalysisResult(const MetaModelAnalysisResult & other) { return new PERSALYS::MetaModelAnalysisResult(other); } 

std::string __repr__() {
  return "MetaModelAnalysisResult";
}

} }
