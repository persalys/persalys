// SWIG file

%{
#include "persalys/DataModel.hxx"
%}

%ignore PERSALYS::DataModel::TypeToString;

%include DataModel_doc.i

%copyctor PERSALYS::DataModel;

%include persalys/DataModel.hxx
