// SWIG file MonteCarloAnalysis.i

%{
#include "persalys/MonteCarloAnalysis.hxx"
%}

%include MonteCarloAnalysis_doc.i

%include persalys/MonteCarloAnalysis.hxx
namespace PERSALYS {

%extend MonteCarloAnalysis { MonteCarloAnalysis(const MonteCarloAnalysis & other) { return new PERSALYS::MonteCarloAnalysis(other); } 

} }
