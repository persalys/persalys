// SWIG file

%{
#include "persalys/SORMAnalysisResult.hxx"
%}

%include SORMAnalysisResult_doc.i

%include persalys/SORMAnalysisResult.hxx
namespace PERSALYS {

%extend SORMAnalysisResult { SORMAnalysisResult(const SORMAnalysisResult & other) { return new PERSALYS::SORMAnalysisResult(other); } 

} }
