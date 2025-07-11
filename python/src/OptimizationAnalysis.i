// SWIG file OptimizationAnalysis.i

%{
#include "persalys/OptimizationAnalysis.hxx"
%}

%include OptimizationAnalysis_doc.i

%ignore PERSALYS::AlgorithmProperty;
%ignore PERSALYS::OptimizationAnalysis::updateParameters;

%copyctor PERSALYS::OptimizationAnalysis;

%include persalys/OptimizationAnalysis.hxx
