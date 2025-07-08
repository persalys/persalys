// SWIG file

%{
#include "persalys/DesignOfExperimentAnalysis.hxx"
%}

%include DesignOfExperimentAnalysis_doc.i

%copyctor PERSALYS::DesignOfExperimentAnalysis;

%include persalys/DesignOfExperimentAnalysis.hxx
namespace PERSALYS {

%extend DesignOfExperimentAnalysis {

std::string __repr__() {
  return "DesignOfExperimentAnalysis";
}

} }
