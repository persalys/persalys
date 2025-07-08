// SWIG file StudyImplementation.i

%{
#include "persalys/StudyImplementation.hxx"
%}

%ignore PERSALYS::StudyImplementation::setFileName;

%copyctor PERSALYS::StudyImplementation;

%include persalys/StudyImplementation.hxx
