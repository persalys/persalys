// SWIG file

%{
#include "persalys/ReliabilityAnalysis.hxx"
%}

%include ReliabilityAnalysis_doc.i

%include persalys/ReliabilityAnalysis.hxx
namespace PERSALYS {

%extend ReliabilityAnalysis { ReliabilityAnalysis(const ReliabilityAnalysis & other) { return new PERSALYS::ReliabilityAnalysis(other); } 

std::string __repr__() {
  return "ReliabilityAnalysis";
}

} }
