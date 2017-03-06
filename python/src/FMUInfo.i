// SWIG file

%{
#include "otgui/FMUInfo.hxx"
%}

%include FMUInfo_doc.i

%include otgui/FMUInfo.hxx
namespace OTGUI {

%extend FMUInfo { FMUInfo(const FMUInfo & other) { return new OTGUI::FMUInfo(other); } 

} }
