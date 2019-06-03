// SWIG file

%{
#include "persalys/FunctionalChaosAnalysisResult.hxx"
%}

%include FunctionalChaosAnalysisResult_doc.i

%include persalys/FunctionalChaosAnalysisResult.hxx
namespace PERSALYS {

%extend FunctionalChaosAnalysisResult { FunctionalChaosAnalysisResult(const FunctionalChaosAnalysisResult & other) { return new PERSALYS::FunctionalChaosAnalysisResult(other); } 

} }
