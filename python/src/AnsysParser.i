// SWIG file

%{
#include "persalys/AnsysParser.hxx"
%}

%include AnsysParser_doc.i

%copyctor PERSALYS::AnsysParser;

%include persalys/AnsysParser.hxx
