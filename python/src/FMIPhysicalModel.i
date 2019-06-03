// SWIG file FMIPhysicalModel.i

%{
#include "persalys/FMIPhysicalModel.hxx"
%}

%include FMIPhysicalModel_doc.i

%include persalys/FMIPhysicalModel.hxx
namespace PERSALYS {

%extend FMIPhysicalModel { FMIPhysicalModel(const FMIPhysicalModel & other) { return new PERSALYS::FMIPhysicalModel(other); } 

} }
