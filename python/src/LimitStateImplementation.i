// SWIG file

%{
#include "persalys/LimitStateImplementation.hxx"
%}

%include LimitStateImplementation_doc.i

%include persalys/LimitStateImplementation.hxx
namespace PERSALYS {

%extend LimitStateImplementation { LimitStateImplementation(const LimitStateImplementation & other) { return new PERSALYS::LimitStateImplementation(other); } 

} }
