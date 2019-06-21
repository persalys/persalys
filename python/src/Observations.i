// SWIG file Observations.i

%{
#include "persalys/Observations.hxx"
%}

%include Observations_doc.i

%include persalys/Observations.hxx
namespace PERSALYS {

%extend Observations { Observations(const Observations & other) { return new PERSALYS::Observations(other); } 

} }
