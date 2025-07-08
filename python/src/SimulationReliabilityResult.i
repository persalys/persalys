// SWIG file

%{
#include "persalys/SimulationReliabilityResult.hxx"
%}

%include SimulationReliabilityResult_doc.i

%ignore PERSALYS::SimulationReliabilityResult::getParameters;

%copyctor PERSALYS::SimulationReliabilityResult;

%include persalys/SimulationReliabilityResult.hxx
