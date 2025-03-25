// SWIG file

%{
#include "persalys/QuantileAnalysis.hxx"
%}

%include QuantileAnalysis_doc.i

%copyctor PERSALYS::QuantileAnalysis;

%include persalys/QuantileAnalysis.hxx

%ignore PERSALYS::QuantileAnalysis::getDefaultTargetProbability;
%ignore PERSALYS::QuantileAnalysis::setDefaultTargetProbability;
%ignore PERSALYS::QuantileAnalysis::checkThresholdCompatibility;
%ignore PERSALYS::QuantileAnalysis::computeWilksValidity;
%ignore PERSALYS::QuantileAnalysis::plotGPD;
%ignore PERSALYS::QuantileAnalysis::plotMeanExcess;
