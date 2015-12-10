// SWIG file

%{
#include "otgui/AnalyticalPhysicalModel.hxx"
%}



%include otgui/AnalyticalPhysicalModel.hxx
namespace OTGUI {

%extend AnalyticalPhysicalModel { AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other) { return new OTGUI::AnalyticalPhysicalModel(other); } 

std::string __repr__() {
  return "AnalyticalPhysicalModel";
}

} }
