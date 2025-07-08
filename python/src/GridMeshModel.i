// SWIG file

%{
#include "persalys/GridMeshModel.hxx"
%}

%include GridMeshModel_doc.i

%copyctor PERSALYS::GridMeshModel;

%include persalys/GridMeshModel.hxx
