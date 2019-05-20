// SWIG file

%{
#include "persalys/AnalysisResult.hxx"
%}

%include AnalysisResult_doc.i

%include persalys/AnalysisResult.hxx
namespace PERSALYS {

%extend AnalysisResult { AnalysisResult(const AnalysisResult & other) { return new PERSALYS::AnalysisResult(other); } 

} }
