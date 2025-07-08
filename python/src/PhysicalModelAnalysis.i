// SWIG file

%{
#include "persalys/PhysicalModelAnalysis.hxx"
%}

%include PhysicalModelAnalysis_doc.i

%copyctor PERSALYS::PhysicalModelAnalysis;

%include persalys/PhysicalModelAnalysis.hxx
namespace PERSALYS {

%extend PhysicalModelAnalysis {

std::string __repr__() {
  return "PhysicalModelAnalysis";
}

} }
