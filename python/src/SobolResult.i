// SWIG file

%{
#include "persalys/SobolResult.hxx"
%}

%include SobolResult_doc.i

%include persalys/SobolResult.hxx
namespace PERSALYS {

%extend SobolResult { SobolResult(const SobolResult & other) { return new PERSALYS::SobolResult(other); } 

} }