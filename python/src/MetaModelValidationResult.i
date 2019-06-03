// SWIG file

%{
#include "persalys/MetaModelValidationResult.hxx"
%}

%ignore *::getParameters;

%include MetaModelValidationResult_doc.i

%include persalys/MetaModelValidationResult.hxx
namespace PERSALYS {

%extend MetaModelValidationResult { MetaModelValidationResult(const MetaModelValidationResult & other) { return new PERSALYS::MetaModelValidationResult(other); } 

std::string __repr__() {
  return "MetaModelValidationResult";
}

} }
