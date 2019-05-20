// SWIG file

%{
#include "persalys/FMUInfo.hxx"
%}

%include FMUInfo_doc.i

%include persalys/FMUInfo.hxx
namespace PERSALYS {

%extend FMUInfo { FMUInfo(const FMUInfo & other) { return new PERSALYS::FMUInfo(other); } 

} }
