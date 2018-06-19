// SWIG file

%{
#include "otgui/ImportedMeshModel.hxx"
%}

%include ImportedMeshModel_doc.i

%include otgui/ImportedMeshModel.hxx
namespace OTGUI {

%extend ImportedMeshModel { ImportedMeshModel(const ImportedMeshModel & other) { return new OTGUI::ImportedMeshModel(other); } 

} }
