// SWIG file

%{
#include "otgui/PhysicalModel.hxx"
%}

OTGUITypedInterfaceObjectHelper(PhysicalModel)

%include otgui/PhysicalModel.hxx
namespace OTGUI {

%extend PhysicalModel { PhysicalModel(const PhysicalModel & other) { return new OTGUI::PhysicalModel(other); } 

std::string __repr__() {
  return "PhysicalModel";
}

} }
