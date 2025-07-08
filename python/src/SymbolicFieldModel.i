// SWIG file

%{
#include "persalys/SymbolicFieldModel.hxx"
%}

%include SymbolicFieldModel_doc.i

%copyctor PERSALYS::SymbolicFieldModel;

%include persalys/SymbolicFieldModel.hxx
