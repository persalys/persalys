// SWIG file

%{
#include "otgui/SymbolicFieldModel.hxx"
%}

%include SymbolicFieldModel_doc.i

%include otgui/SymbolicFieldModel.hxx
namespace OTGUI {

%extend SymbolicFieldModel { SymbolicFieldModel(const SymbolicFieldModel & other) { return new OTGUI::SymbolicFieldModel(other); } 

} }
