// SWIG file

%{
#include "otgui/WithStopCriteriaAnalysis.hxx"
%}

%include WithStopCriteriaAnalysis_doc.i

%include otgui/WithStopCriteriaAnalysis.hxx
namespace OTGUI {

%extend WithStopCriteriaAnalysis { WithStopCriteriaAnalysis(const WithStopCriteriaAnalysis & other) { return new OTGUI::WithStopCriteriaAnalysis(other); } 

} }
