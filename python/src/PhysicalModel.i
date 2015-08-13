// SWIG file

%{
#include "PhysicalModel.hxx"
%}

OTGUITypedInterfaceObjectHelper(PhysicalModel)

%include PhysicalModel.hxx
namespace OTGUI {

%extend PhysicalModel { PhysicalModel(const PhysicalModel & other) { return new OTGUI::PhysicalModel(other); } 

std::string __repr__() {
  return "PhysicalModel";
}

} }
