// SWIG file

%{
#include "persalys/FMUInfo.hxx"
%}

%include FMUInfo_doc.i

%copyctor PERSALYS::FMUInfo;

%include persalys/FMUInfo.hxx
