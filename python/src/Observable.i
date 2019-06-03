// SWIG file Observable.i

%{
#include "persalys/Observable.hxx"
%}

%ignore Observable;

%include persalys/Observable.hxx


namespace PERSALYS {

%extend Observable { Observable(const Observable & other) { return new PERSALYS::Observable(other); } 

}

}
