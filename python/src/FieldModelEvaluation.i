// SWIG file

%{
#include "otgui/FieldModelEvaluation.hxx"
%}

%include FieldModelEvaluation_doc.i

%include otgui/FieldModelEvaluation.hxx
namespace OTGUI {

%extend FieldModelEvaluation { FieldModelEvaluation(const FieldModelEvaluation & other) { return new OTGUI::FieldModelEvaluation(other); } 

} }
