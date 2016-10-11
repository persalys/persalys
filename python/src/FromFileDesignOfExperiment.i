// SWIG file

%{
#include "otgui/FromFileDesignOfExperiment.hxx"
%}

%include FromFileDesignOfExperiment_doc.i

%include otgui/FromFileDesignOfExperiment.hxx
namespace OTGUI {

%extend FromFileDesignOfExperiment { FromFileDesignOfExperiment(const FromFileDesignOfExperiment & other) { return new OTGUI::FromFileDesignOfExperiment(other); } 

} }
