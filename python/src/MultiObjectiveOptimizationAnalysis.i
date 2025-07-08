// SWIG file MultiObjectiveOptimizationAnalysis.i

%{
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
%}

%include MultiObjectiveOptimizationAnalysis_doc.i

%ignore PERSALYS::MultiObjectiveOptimizationAnalysis::updateParameters;

%copyctor PERSALYS::MultiObjectiveOptimizationAnalysis;

%include persalys/MultiObjectiveOptimizationAnalysis.hxx
