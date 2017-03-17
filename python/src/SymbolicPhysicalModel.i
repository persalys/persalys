// SWIG file

%{
#include "otgui/SymbolicPhysicalModel.hxx"
%}

%include SymbolicPhysicalModel_doc.i

%include otgui/SymbolicPhysicalModel.hxx
namespace OTGUI {

%extend SymbolicPhysicalModel { SymbolicPhysicalModel(const SymbolicPhysicalModel & other) { return new OTGUI::SymbolicPhysicalModel(other); } 

} }
