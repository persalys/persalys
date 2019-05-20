// SWIG file

%{
#include "persalys/ProbabilisticDesignOfExperiment.hxx"
%}

%ignore PERSALYS::ProbabilisticDesignOfExperiment::setDesignOfExperiment;

%include ProbabilisticDesignOfExperiment_doc.i

%include persalys/ProbabilisticDesignOfExperiment.hxx
namespace PERSALYS {

%extend ProbabilisticDesignOfExperiment { ProbabilisticDesignOfExperiment(const ProbabilisticDesignOfExperiment & other) { return new PERSALYS::ProbabilisticDesignOfExperiment(other); } 

} }
