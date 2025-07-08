// SWIG file

%{
#include "persalys/ReliabilityAnalysis.hxx"
%}

%include ReliabilityAnalysis_doc.i

%copyctor PERSALYS::ReliabilityAnalysis;

%include persalys/ReliabilityAnalysis.hxx
namespace PERSALYS {

%extend ReliabilityAnalysis {

std::string __repr__() {
  return "ReliabilityAnalysis";
}

} }
