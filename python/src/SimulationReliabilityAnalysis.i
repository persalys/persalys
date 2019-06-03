// SWIG file SimulationReliabilityAnalysis.i

%{
#include "persalys/SimulationReliabilityAnalysis.hxx"
%}

%include SimulationReliabilityAnalysis_doc.i

%include persalys/SimulationReliabilityAnalysis.hxx
namespace PERSALYS {

%extend SimulationReliabilityAnalysis { SimulationReliabilityAnalysis(const SimulationReliabilityAnalysis & other) { return new PERSALYS::SimulationReliabilityAnalysis(other); } 

} }
