// SWIG file

%{
#include "persalys/MetaModelAnalysisResult.hxx"
%}

%include MetaModelAnalysisResult_doc.i

%copyctor PERSALYS::MetaModelAnalysisResult;

%include persalys/MetaModelAnalysisResult.hxx
namespace PERSALYS {

%extend MetaModelAnalysisResult {

std::string __repr__() {
  return "MetaModelAnalysisResult";
}

} }
