// SWIG file

%{
#include "persalys/DataSample.hxx"
%}

%include DataSample_doc.i

%copyctor PERSALYS::DataSample;

%include persalys/DataSample.hxx
