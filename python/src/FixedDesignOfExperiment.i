// SWIG file

%{
#include "persalys/FixedDesignOfExperiment.hxx"
%}

%ignore PERSALYS::FixedDesignOfExperiment::setDesignOfExperiment;

%include FixedDesignOfExperiment_doc.i

%copyctor PERSALYS::FixedDesignOfExperiment;

%include persalys/FixedDesignOfExperiment.hxx
