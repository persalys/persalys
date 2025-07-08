// SWIG file

%{
#include "persalys/DataModel.hxx"
%}

%include DataModel_doc.i

%copyctor PERSALYS::DataModel;

%include persalys/DataModel.hxx
