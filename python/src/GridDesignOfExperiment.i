// SWIG file

%{
#include "otgui/GridDesignOfExperiment.hxx"
%}

%include GridDesignOfExperiment_doc.i

%ignore OTGUI::GridDesignOfExperiment::updateParameters;

%include otgui/GridDesignOfExperiment.hxx
namespace OTGUI {

%extend GridDesignOfExperiment { GridDesignOfExperiment(const GridDesignOfExperiment & other) { return new OTGUI::GridDesignOfExperiment(other); } 

} }
