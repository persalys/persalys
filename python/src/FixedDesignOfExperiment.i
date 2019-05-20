// SWIG file

%{
#include "persalys/FixedDesignOfExperiment.hxx"
%}

%ignore PERSALYS::FixedDesignOfExperiment::setDesignOfExperiment;

%include FixedDesignOfExperiment_doc.i

%include persalys/FixedDesignOfExperiment.hxx
namespace PERSALYS {

%extend FixedDesignOfExperiment { FixedDesignOfExperiment(const FixedDesignOfExperiment & other) { return new PERSALYS::FixedDesignOfExperiment(other); } 

} }
