// SWIG file

%{
#include "persalys/SymbolicPhysicalModel.hxx"
%}

%include SymbolicPhysicalModel_doc.i

%include persalys/SymbolicPhysicalModel.hxx
namespace PERSALYS {

%extend SymbolicPhysicalModel { SymbolicPhysicalModel(const SymbolicPhysicalModel & other) { return new PERSALYS::SymbolicPhysicalModel(other); } 

} }
