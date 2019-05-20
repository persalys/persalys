// SWIG file MeshModelImplementation.i

%{
#include "persalys/MeshModelImplementation.hxx"
%}

%ignore *::getHtmlDescription;

%include MeshModelImplementation_doc.i

%include persalys/MeshModelImplementation.hxx
namespace PERSALYS {

%extend MeshModelImplementation { MeshModelImplementation(const MeshModelImplementation & other) { return new PERSALYS::MeshModelImplementation(other); } 

} }
