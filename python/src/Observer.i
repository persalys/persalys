// SWIG file

%{
#include "persalys/Observer.hxx"
%}

%copyctor PERSALYS::Observer;

%include persalys/Observer.hxx
namespace PERSALYS {

%extend Observer {

std::string __repr__() {
  return "Observer";
}

} }
