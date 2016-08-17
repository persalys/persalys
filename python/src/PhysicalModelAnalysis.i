// SWIG file

%{
#include "otgui/PhysicalModelAnalysis.hxx"
%}

%ignore PhysicalModelAnalysis;

%include otgui/PhysicalModelAnalysis.hxx
namespace OTGUI {

%extend PhysicalModelAnalysis { PhysicalModelAnalysis(const PhysicalModelAnalysis & other) { return new OTGUI::PhysicalModelAnalysis(other); } 

std::string __repr__() {
  return "PhysicalModelAnalysis";
}

} }
