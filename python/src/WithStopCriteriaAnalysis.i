// SWIG file

%{
#include "otgui/WithStopCriteriaAnalysis.hxx"
%}

%include WithStopCriteriaAnalysis_doc.i

%ignore OTGUI::WithStopCriteriaAnalysis::TimeCriteria;

%include otgui/WithStopCriteriaAnalysis.hxx
namespace OTGUI {

%extend WithStopCriteriaAnalysis { WithStopCriteriaAnalysis(const WithStopCriteriaAnalysis & other) { return new OTGUI::WithStopCriteriaAnalysis(other); } 

} }
