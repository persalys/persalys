// SWIG file

#ifdef OTGUI_HAVE_YACS
%{
#include "otgui/YACSPhysicalModel.hxx"
%}



%include otgui/YACSPhysicalModel.hxx
namespace OTGUI {

%extend YACSPhysicalModel { YACSPhysicalModel(const YACSPhysicalModel & other) { return new OTGUI::YACSPhysicalModel(other); } 

std::string __repr__() {
  return "YACSPhysicalModel";
}

} }
#endif
