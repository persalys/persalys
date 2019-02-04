// SWIG file

%{
#include "otgui/FORMAnalysisResult.hxx"
%}

%include FORMAnalysisResult_doc.i

%include otgui/FORMAnalysisResult.hxx
namespace OTGUI {

%extend FORMAnalysisResult { FORMAnalysisResult(const FORMAnalysisResult & other) { return new OTGUI::FORMAnalysisResult(other); } 

} }
