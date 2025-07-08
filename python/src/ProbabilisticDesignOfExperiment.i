// SWIG file

%{
#include "persalys/ProbabilisticDesignOfExperiment.hxx"
%}

%ignore PERSALYS::ProbabilisticDesignOfExperiment::setDesignOfExperiment;

%include ProbabilisticDesignOfExperiment_doc.i

%copyctor PERSALYS::ProbabilisticDesignOfExperiment;

%include persalys/ProbabilisticDesignOfExperiment.hxx
