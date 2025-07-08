// SWIG file

%{
#include "persalys/SimulationAnalysis.hxx"
%}

%include SimulationAnalysis_doc.i

%ignore PERSALYS::SimulationAnalysis::getParameters;

%copyctor PERSALYS::SimulationAnalysis;

%include persalys/SimulationAnalysis.hxx
namespace PERSALYS {

%extend SimulationAnalysis {

std::string __repr__() {
  return "SimulationAnalysis";
}

} }
