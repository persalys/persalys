// SWIG file

%{
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"
%}

%include TaylorExpansionMomentsAnalysis_doc.i

%include otgui/TaylorExpansionMomentsAnalysis.hxx
namespace OTGUI {

%extend TaylorExpansionMomentsAnalysis { TaylorExpansionMomentsAnalysis(const TaylorExpansionMomentsAnalysis & other) { return new OTGUI::TaylorExpansionMomentsAnalysis(other); } 

std::string __repr__() {
  return "TaylorExpansionMomentsAnalysis";
}

} }
