// SWIG file

%{
#include "otgui/SymbolicFieldModel.hxx"
%}

%include otgui/SymbolicFieldModel.hxx
namespace OTGUI {

%extend SymbolicFieldModel { SymbolicFieldModel(const SymbolicFieldModel & other) { return new OTGUI::SymbolicFieldModel(other); } 

} }
