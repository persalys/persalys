// SWIG file

%{
#include "persalys/DesignOfExperimentImplementation.hxx"
%}

%include DesignOfExperimentImplementation_doc.i

%include persalys/DesignOfExperimentImplementation.hxx
namespace PERSALYS {

%extend DesignOfExperimentImplementation { DesignOfExperimentImplementation(const DesignOfExperimentImplementation & other) { return new PERSALYS::DesignOfExperimentImplementation(other); } 

std::string __repr__() {
  return "DesignOfExperimentImplementation";
}

} }
