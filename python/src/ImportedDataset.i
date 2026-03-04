// SWIG file

%{
#include "persalys/ImportedDataset.hxx"
%}

%include ImportedDataset_doc.i

%copyctor PERSALYS::ImportedDataset;

%include persalys/ImportedDataset.hxx