// SWIG file

%{
#include "persalys/MetaModel.hxx"
%}

%include MetaModel_doc.i

%copyctor PERSALYS::MetaModel;

%include persalys/MetaModel.hxx
