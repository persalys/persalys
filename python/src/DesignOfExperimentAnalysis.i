// SWIG file

%{
#include "otgui/DesignOfExperimentAnalysis.hxx"
%}

%include otgui/DesignOfExperimentAnalysis.hxx
namespace OTGUI {

%extend DesignOfExperimentAnalysis { DesignOfExperimentAnalysis(const DesignOfExperimentAnalysis & other) { return new OTGUI::DesignOfExperimentAnalysis(other); } 

std::string __repr__() {
  return "DesignOfExperimentAnalysis";
}

} }
