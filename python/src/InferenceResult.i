// SWIG file

%{
#include "otgui/InferenceResult.hxx"
%}

%include InferenceResult_doc.i

%include otgui/InferenceResult.hxx
namespace OTGUI {

%extend InferenceResult { InferenceResult(const InferenceResult & other) { return new OTGUI::InferenceResult(other); } 

} }
