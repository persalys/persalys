// SWIG file MonteCarloReliabilityAnalysis.i

%{
#include "persalys/MonteCarloReliabilityAnalysis.hxx"
%}

%include MonteCarloReliabilityAnalysis_doc.i

%include persalys/MonteCarloReliabilityAnalysis.hxx
namespace PERSALYS {

%extend MonteCarloReliabilityAnalysis { MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other) { return new PERSALYS::MonteCarloReliabilityAnalysis(other); } 

} }
