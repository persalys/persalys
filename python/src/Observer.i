// SWIG file

%{
#include "persalys/Observer.hxx"
%}


%include persalys/Observer.hxx
namespace PERSALYS {

%extend Observer { Observer(const Observer & other) { return new PERSALYS::Observer(other); } 

std::string __repr__() {
  return "Observer";
}

} }
