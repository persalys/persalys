// SWIG file

%{
#include "persalys/SimulationReliabilityResult.hxx"
%}

%include SimulationReliabilityResult_doc.i

%ignore PERSALYS::SimulationReliabilityResult::getParameters;

%include persalys/SimulationReliabilityResult.hxx
namespace PERSALYS {

%extend SimulationReliabilityResult { SimulationReliabilityResult(const SimulationReliabilityResult & other) { return new PERSALYS::SimulationReliabilityResult(other); } 

} }
