// SWIG file

%{
#include "persalys/WithStopCriteriaAnalysis.hxx"
%}

%include WithStopCriteriaAnalysis_doc.i

%ignore PERSALYS::WithStopCriteriaAnalysis::TimeCriteria;

%include persalys/WithStopCriteriaAnalysis.hxx
namespace PERSALYS {

%extend WithStopCriteriaAnalysis { WithStopCriteriaAnalysis(const WithStopCriteriaAnalysis & other) { return new PERSALYS::WithStopCriteriaAnalysis(other); } 

} }
