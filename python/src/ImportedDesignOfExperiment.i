// SWIG file

%{
#include "persalys/ImportedDesignOfExperiment.hxx"
%}

%ignore PERSALYS::ImportedDesignOfExperiment::setDesignOfExperiment;

%include ImportedDesignOfExperiment_doc.i

%include persalys/ImportedDesignOfExperiment.hxx
namespace PERSALYS {

%extend ImportedDesignOfExperiment { ImportedDesignOfExperiment(const ImportedDesignOfExperiment & other) { return new PERSALYS::ImportedDesignOfExperiment(other); } 

} }
