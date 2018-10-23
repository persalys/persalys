// SWIG file

%{
#include "otgui/MetaModelValidationResult.hxx"
%}

%ignore *::getParameters;

%include MetaModelValidationResult_doc.i

%include otgui/MetaModelValidationResult.hxx
namespace OTGUI {

%extend MetaModelValidationResult { MetaModelValidationResult(const MetaModelValidationResult & other) { return new OTGUI::MetaModelValidationResult(other); } 

std::string __repr__() {
  return "MetaModelValidationResult";
}

} }
