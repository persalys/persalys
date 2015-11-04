// SWIG file

%{
#include "otgui/MonteCarloResult.hxx"
%}

%include MonteCarloResult_doc.i

%include otgui/MonteCarloResult.hxx
namespace OTGUI {

%extend MonteCarloResult { MonteCarloResult(const MonteCarloResult & other) { return new OTGUI::MonteCarloResult(other); } 

std::string __repr__() {
  return "MonteCarloResult";
}

} }
