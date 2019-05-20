// SWIG file

%{
#include "persalys/GridMeshModel.hxx"
%}

%include GridMeshModel_doc.i

%include persalys/GridMeshModel.hxx
namespace PERSALYS {

%extend GridMeshModel { GridMeshModel(const GridMeshModel & other) { return new PERSALYS::GridMeshModel(other); } 

} }
