// SWIG file DesignOfExperimentEvaluation.i

%{
#include "persalys/DesignOfExperimentEvaluation.hxx"
%}

%ignore PERSALYS::DesignOfExperimentEvaluation::setDesignOfExperiment;

%include DesignOfExperimentEvaluation_doc.i

%include persalys/DesignOfExperimentEvaluation.hxx
namespace PERSALYS {

%extend DesignOfExperimentEvaluation { DesignOfExperimentEvaluation(const DesignOfExperimentEvaluation & other) { return new PERSALYS::DesignOfExperimentEvaluation(other); } 

} }
