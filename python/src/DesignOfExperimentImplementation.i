// SWIG file

%{
#include "persalys/DesignOfExperimentImplementation.hxx"
%}

%include DesignOfExperimentImplementation_doc.i

%copyctor PERSALYS::DesignOfExperimentImplementation;

%include persalys/DesignOfExperimentImplementation.hxx
namespace PERSALYS {

%extend DesignOfExperimentImplementation {

std::string __repr__() {
  return "DesignOfExperimentImplementation";
}

} }
