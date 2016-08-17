// SWIG file

%{
#include "otgui/DataModelAnalysis.hxx"
%}

%ignore DataModelAnalysis;

%include otgui/DataModelAnalysis.hxx
namespace OTGUI {

%extend DataModelAnalysis { DataModelAnalysis(const DataModelAnalysis & other) { return new OTGUI::DataModelAnalysis(other); } 

std::string __repr__() {
  return "DataModelAnalysis";
}

} }
