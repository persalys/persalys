// SWIG file

%{
#include "persalys/WithStopCriteriaAnalysis.hxx"
%}

%include WithStopCriteriaAnalysis_doc.i

%ignore PERSALYS::WithStopCriteriaAnalysis::TimeCriteria;
%ignore PERSALYS::WithStopCriteriaAnalysis::getParameters;

%copyctor PERSALYS::WithStopCriteriaAnalysis;

%include persalys/WithStopCriteriaAnalysis.hxx
