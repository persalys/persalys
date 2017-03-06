// SWIG file FMIPhysicalModel.i

%{
#include "otgui/FMIPhysicalModel.hxx"
%}

%include FMIPhysicalModel_doc.i

%include otgui/FMIPhysicalModel.hxx
namespace OTGUI {

%extend FMIPhysicalModel { FMIPhysicalModel(const FMIPhysicalModel & other) { return new OTGUI::FMIPhysicalModel(other); } 

} }
