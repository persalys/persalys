// SWIG file

%{
#include "persalys/SimulationAnalysis.hxx"
%}

%include SimulationAnalysis_doc.i

%include persalys/SimulationAnalysis.hxx
namespace PERSALYS {

%extend SimulationAnalysis { SimulationAnalysis(const SimulationAnalysis & other) { return new PERSALYS::SimulationAnalysis(other); } 

std::string __repr__() {
  return "SimulationAnalysis";
}

} }
