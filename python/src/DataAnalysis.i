// SWIG file

%{
#include "otgui/DataAnalysis.hxx"
%}

%include otgui/DataAnalysis.hxx
namespace OTGUI {

%extend DataAnalysis { DataAnalysis(const DataAnalysis & other) { return new OTGUI::DataAnalysis(other); } 

std::string __repr__() {
  return "DataAnalysis";
}

} }
