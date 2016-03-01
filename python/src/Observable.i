// SWIG file Observable.i

%{
#include "otgui/Observable.hxx"
%}

%ignore *::addObserver;
%ignore *::removeObserver;
%ignore *::blockNotification;
%ignore *::getObservers;
%ignore *::setObservers;
%ignore *::notify;

%include otgui/Observable.hxx


namespace OTGUI {

%extend Observable { Observable(const Observable & other) { return new OTGUI::Observable(other); } 

}

}
