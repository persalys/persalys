// SWIG file StudyImplementation.i

%{
#include "persalys/StudyImplementation.hxx"
%}

%ignore PERSALYS::StudyImplementation::setFileName;

%include persalys/StudyImplementation.hxx
namespace PERSALYS {

%extend StudyImplementation { StudyImplementation(const StudyImplementation & other) { return new PERSALYS::StudyImplementation(other); } 

} }
