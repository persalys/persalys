// SWIG file

%{
#include "otgui/ReliabilityAnalysis.hxx"
%}


%include otgui/ReliabilityAnalysis.hxx
namespace OTGUI {

%extend ReliabilityAnalysis { ReliabilityAnalysis(const ReliabilityAnalysis & other) { return new OTGUI::ReliabilityAnalysis(other); } 

std::string __repr__() {
  return "ReliabilityAnalysis";
}

} }
