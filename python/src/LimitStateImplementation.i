// SWIG file

%{
#include "persalys/LimitStateImplementation.hxx"
%}

%include LimitStateImplementation_doc.i

%copyctor PERSALYS::LimitStateImplementation;

%include persalys/LimitStateImplementation.hxx
