// SWIG file

%{
#include "otgui/ImportedSample.hxx"
%}

%include ImportedSample_doc.i

%ignore ImportedSample;

%include otgui/ImportedSample.hxx
namespace OTGUI {

%extend ImportedSample { ImportedSample(const ImportedSample & other) { return new OTGUI::ImportedSample(other); } 

} }
