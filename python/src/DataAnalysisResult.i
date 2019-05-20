// SWIG file

%{
#include "persalys/DataAnalysisResult.hxx"
%}

%include DataAnalysisResult_doc.i

%include persalys/DataAnalysisResult.hxx
namespace PERSALYS {

%extend DataAnalysisResult { DataAnalysisResult(const DataAnalysisResult & other) { return new PERSALYS::DataAnalysisResult(other); } 

} }
