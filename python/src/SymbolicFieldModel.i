// SWIG file

%{
#include "persalys/SymbolicFieldModel.hxx"
%}

%include SymbolicFieldModel_doc.i

%include persalys/SymbolicFieldModel.hxx
namespace PERSALYS {

%extend SymbolicFieldModel { SymbolicFieldModel(const SymbolicFieldModel & other) { return new PERSALYS::SymbolicFieldModel(other); } 

} }
