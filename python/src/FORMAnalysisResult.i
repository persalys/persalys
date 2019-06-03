// SWIG file

%{
#include "persalys/FORMAnalysisResult.hxx"
%}

%include FORMAnalysisResult_doc.i

%include persalys/FORMAnalysisResult.hxx
namespace PERSALYS {

%extend FORMAnalysisResult { FORMAnalysisResult(const FORMAnalysisResult & other) { return new PERSALYS::FORMAnalysisResult(other); } 

} }
