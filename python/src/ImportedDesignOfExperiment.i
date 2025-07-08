// SWIG file

%{
#include "persalys/ImportedDesignOfExperiment.hxx"
%}

%ignore PERSALYS::ImportedDesignOfExperiment::setDesignOfExperiment;

%include ImportedDesignOfExperiment_doc.i

%copyctor PERSALYS::ImportedDesignOfExperiment;

%include persalys/ImportedDesignOfExperiment.hxx
