// SWIG file

%{
#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"
%}



%include otgui/TaylorExpansionsMomentsAnalysis.hxx
namespace OTGUI {

%extend TaylorExpansionsMomentsAnalysis { TaylorExpansionsMomentsAnalysis(const TaylorExpansionsMomentsAnalysis & other) { return new OTGUI::TaylorExpansionsMomentsAnalysis(other); } 

std::string __repr__() {
  return "TaylorExpansionsMomentsAnalysis";
}

} }
