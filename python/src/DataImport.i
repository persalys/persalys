// SWIG file

%{
#include "persalys/DataImport.hxx"
%}

%include DataImport_doc.i

%copyctor PERSALYS::DataImport;

%include persalys/DataImport.hxx
