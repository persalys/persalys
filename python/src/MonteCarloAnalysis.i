// SWIG file MonteCarloAnalysis.i

%{
#include "otgui/MonteCarloAnalysis.hxx"
%}

%include MonteCarloAnalysis_doc.i

%include otgui/MonteCarloAnalysis.hxx
namespace OTGUI {

%extend MonteCarloAnalysis { MonteCarloAnalysis(const MonteCarloAnalysis & other) { return new OTGUI::MonteCarloAnalysis(other); } 

std::string __repr__() {
  return "MonteCarloAnalysis";
}

} }
