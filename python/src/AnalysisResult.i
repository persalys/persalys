// SWIG file

%{
#include "otgui/AnalysisResult.hxx"
%}

%include AnalysisResult_doc.i

%include otgui/AnalysisResult.hxx
namespace OTGUI {

%extend AnalysisResult { AnalysisResult(const AnalysisResult & other) { return new OTGUI::AnalysisResult(other); } 

} }
