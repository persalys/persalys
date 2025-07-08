// SWIG file

%{
#include "persalys/SymbolicPhysicalModel.hxx"
%}

%include SymbolicPhysicalModel_doc.i

%copyctor PERSALYS::SymbolicPhysicalModel;

%include persalys/SymbolicPhysicalModel.hxx
