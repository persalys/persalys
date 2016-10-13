// SWIG file

%{
#include "otgui/MonteCarloReliabilityResult.hxx"
%}

%include MonteCarloReliabilityResult_doc.i

%include otgui/MonteCarloReliabilityResult.hxx
namespace OTGUI {

%extend MonteCarloReliabilityResult { MonteCarloReliabilityResult(const MonteCarloReliabilityResult & other) { return new OTGUI::MonteCarloReliabilityResult(other); } 

} }
