// SWIG file

%{
#include "otgui/DataAnalysisResult.hxx"
%}

%include DataAnalysisResult_doc.i

%include otgui/DataAnalysisResult.hxx
namespace OTGUI {

%extend DataAnalysisResult { DataAnalysisResult(const DataAnalysisResult & other) { return new OTGUI::DataAnalysisResult(other); } 

} }
