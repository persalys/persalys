// SWIG file FileMemoizeEvaluation.i

%{
#include "persalys/FileMemoizeEvaluation.hxx"
%}

%include persalys/FileMemoizeEvaluation.hxx
namespace PERSALYS{ %extend FileMemoizeEvaluation { FileMemoizeEvaluation(const FileMemoizeEvaluation & other) { return new PERSALYS::FileMemoizeEvaluation(other); } } }
