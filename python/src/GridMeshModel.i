// SWIG file

%{
#include "otgui/GridMeshModel.hxx"
%}

%include GridMeshModel_doc.i

%include otgui/GridMeshModel.hxx
namespace OTGUI {

%extend GridMeshModel { GridMeshModel(const GridMeshModel & other) { return new OTGUI::GridMeshModel(other); } 

} }
