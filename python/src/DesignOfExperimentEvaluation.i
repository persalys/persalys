// SWIG file DesignOfExperimentEvaluation.i

%{
#include "otgui/DesignOfExperimentEvaluation.hxx"
%}


%include otgui/DesignOfExperimentEvaluation.hxx
namespace OTGUI {

%extend DesignOfExperimentEvaluation { DesignOfExperimentEvaluation(const DesignOfExperimentEvaluation & other) { return new OTGUI::DesignOfExperimentEvaluation(other); } 

} }
