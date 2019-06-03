// SWIG file

%{
#include "persalys/DesignOfExperimentAnalysis.hxx"
%}

%include DesignOfExperimentAnalysis_doc.i

%include persalys/DesignOfExperimentAnalysis.hxx
namespace PERSALYS {

%extend DesignOfExperimentAnalysis { DesignOfExperimentAnalysis(const DesignOfExperimentAnalysis & other) { return new PERSALYS::DesignOfExperimentAnalysis(other); } 

std::string __repr__() {
  return "DesignOfExperimentAnalysis";
}

} }
