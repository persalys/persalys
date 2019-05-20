// SWIG file

%{
#include "persalys/PhysicalModelAnalysis.hxx"
%}

%include PhysicalModelAnalysis_doc.i

%include persalys/PhysicalModelAnalysis.hxx
namespace PERSALYS {

%extend PhysicalModelAnalysis { PhysicalModelAnalysis(const PhysicalModelAnalysis & other) { return new PERSALYS::PhysicalModelAnalysis(other); } 

std::string __repr__() {
  return "PhysicalModelAnalysis";
}

} }
