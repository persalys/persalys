// SWIG file Observable.i

%{
#include "persalys/Observable.hxx"
%}

%ignore PERSALYS::Observable::operator=;
%ignore PERSALYS::Observable::addObserver;
%ignore PERSALYS::Observable::blockNotification;
%ignore PERSALYS::Observable::getObserver;
%ignore PERSALYS::Observable::getObservers;
%ignore PERSALYS::Observable::notify;
%ignore PERSALYS::Observable::notifyAndRemove;
%ignore PERSALYS::Observable::removeAllObservers;
%ignore PERSALYS::Observable::removeObserver;

%include persalys/Observable.hxx


