// SWIG file

%{
#include "persalys/SobolResult.hxx"
%}

%include SobolResult_doc.i

%copyctor PERSALYS::SobolResult;

%include persalys/SobolResult.hxx
