// SWIG file

%{
#include "otgui/MonteCarloCalculus.hxx"
%}



%include otgui/MonteCarloCalculus.hxx
namespace OTGUI {

%extend MonteCarloCalculus { MonteCarloCalculus(const MonteCarloCalculus & other) { return new OTGUI::MonteCarloCalculus(other); } 

std::string __repr__() {
  return "MonteCarloCalculus";
}

} }
