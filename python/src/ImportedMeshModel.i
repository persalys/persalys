// SWIG file

%{
#include "persalys/ImportedMeshModel.hxx"
%}

%include ImportedMeshModel_doc.i

%copyctor PERSALYS::ImportedMeshModel;

%include persalys/ImportedMeshModel.hxx
