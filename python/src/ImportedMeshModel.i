// SWIG file

%{
#include "persalys/ImportedMeshModel.hxx"
%}

%include ImportedMeshModel_doc.i

%include persalys/ImportedMeshModel.hxx
namespace PERSALYS {

%extend ImportedMeshModel { ImportedMeshModel(const ImportedMeshModel & other) { return new PERSALYS::ImportedMeshModel(other); } 

} }
