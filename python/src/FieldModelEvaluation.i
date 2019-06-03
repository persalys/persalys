// SWIG file

%{
#include "persalys/FieldModelEvaluation.hxx"
%}

%include FieldModelEvaluation_doc.i

%include persalys/FieldModelEvaluation.hxx
namespace PERSALYS {

%extend FieldModelEvaluation { FieldModelEvaluation(const FieldModelEvaluation & other) { return new PERSALYS::FieldModelEvaluation(other); } 

} }
