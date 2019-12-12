// SWIG file FileMemoizeFunction.i

%{
#include "persalys/FileMemoizeFunction.hxx"
%}

%include FileMemoizeFunction_doc.i

%include persalys/FileMemoizeFunction.hxx
namespace PERSALYS{ %extend FileMemoizeFunction { FileMemoizeFunction(const FileMemoizeFunction & other) { return new PERSALYS::FileMemoizeFunction(other); } } }
