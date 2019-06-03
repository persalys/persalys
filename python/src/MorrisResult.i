// SWIG file

%{
#include "persalys/MorrisResult.hxx"
%}

%include MorrisResult_doc.i

%include persalys/MorrisResult.hxx
namespace PERSALYS {

%extend MorrisResult { MorrisResult(const MorrisResult & other) { return new PERSALYS::MorrisResult(other); } 

} }
