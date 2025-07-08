// SWIG file DesignOfExperimentEvaluation.i

%{
#include "persalys/DesignOfExperimentEvaluation.hxx"
%}

%ignore PERSALYS::DesignOfExperimentEvaluation::setDesignOfExperiment;

%include DesignOfExperimentEvaluation_doc.i

%copyctor PERSALYS::DesignOfExperimentEvaluation;

%include persalys/DesignOfExperimentEvaluation.hxx
