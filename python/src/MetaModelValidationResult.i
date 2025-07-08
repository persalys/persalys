// SWIG file

%{
#include "persalys/MetaModelValidationResult.hxx"
%}

%ignore *::getParameters;

%include MetaModelValidationResult_doc.i

%copyctor PERSALYS::MetaModelValidationResult;

%include persalys/MetaModelValidationResult.hxx
namespace PERSALYS {

%extend MetaModelValidationResult {

std::string __repr__() {
  return "MetaModelValidationResult";
}

} }
