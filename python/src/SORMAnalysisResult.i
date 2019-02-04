// SWIG file

%{
#include "otgui/SORMAnalysisResult.hxx"
%}

%include SORMAnalysisResult_doc.i

%include otgui/SORMAnalysisResult.hxx
namespace OTGUI {

%extend SORMAnalysisResult { SORMAnalysisResult(const SORMAnalysisResult & other) { return new OTGUI::SORMAnalysisResult(other); } 

} }
