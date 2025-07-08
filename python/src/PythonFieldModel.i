// SWIG file

%{
#include "persalys/PythonFieldModel.hxx"
%}

%include PythonFieldModel_doc.i

%copyctor PERSALYS::PythonFieldModel;

%include persalys/PythonFieldModel.hxx
