// SWIG file MeshModelImplementation.i

%{
#include "persalys/MeshModelImplementation.hxx"
%}

%ignore *::getHTMLDescription;

%include MeshModelImplementation_doc.i

%copyctor PERSALYS::MeshModelImplementation;

%include persalys/MeshModelImplementation.hxx
