// SWIG file Observable.i

%{
#include "otgui/Observable.hxx"
%}

%ignore Observable;

%include otgui/Observable.hxx


namespace OTGUI {

%extend Observable { Observable(const Observable & other) { return new OTGUI::Observable(other); } 

}

}
