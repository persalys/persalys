// SWIG file

%{
#include "otgui/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i

%include otgui/YACSPhysicalModel.hxx
namespace OTGUI {

%extend YACSPhysicalModel { YACSPhysicalModel(const YACSPhysicalModel & other) { return new OTGUI::YACSPhysicalModel(other); } 

} }
