// SWIG file

%{
#include "otgui/QuadraticCumulAnalysis.hxx"
%}



%include otgui/QuadraticCumulAnalysis.hxx
namespace OTGUI {

%extend QuadraticCumulAnalysis { QuadraticCumulAnalysis(const QuadraticCumulAnalysis & other) { return new OTGUI::QuadraticCumulAnalysis(other); } 

std::string __repr__() {
  return "QuadraticCumulAnalysis";
}

} }
