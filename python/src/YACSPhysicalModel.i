// SWIG file

%{
#include "YACSPhysicalModel.hxx"
%}



%include YACSPhysicalModel.hxx
namespace OTGUI {

%extend YACSPhysicalModel { YACSPhysicalModel(const YACSPhysicalModel & other) { return new OTGUI::YACSPhysicalModel(other); } 

std::string __repr__() {
  return "YACSPhysicalModel";
}

} }
