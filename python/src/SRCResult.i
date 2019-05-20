// SWIG file

%{
#include "persalys/SRCResult.hxx"
%}

%include SRCResult_doc.i

%include persalys/SRCResult.hxx
namespace PERSALYS {

%extend SRCResult { SRCResult(const SRCResult & other) { return new PERSALYS::SRCResult(other); } 

} }