// SWIG file MeshModelImplementation.i

%{
#include "otgui/MeshModelImplementation.hxx"
%}

%ignore *::getHtmlDescription;

%include MeshModelImplementation_doc.i

%include otgui/MeshModelImplementation.hxx
namespace OTGUI {

%extend MeshModelImplementation { MeshModelImplementation(const MeshModelImplementation & other) { return new OTGUI::MeshModelImplementation(other); } 

} }
