#include "Observable.hxx"
#include <iostream>
namespace OTGUI {

Observable::Observable()
{
}

void Observable::addObserver(Observer * observer)
{
  observers_.push_back(observer);
}


void Observable::notify(const std::string & message)
{
  for (unsigned int i = 0; i < observers_.size(); ++ i)
    observers_[i]->update(this, message);
}


std::vector< Observer* > Observable::getObserver() const
{
  return observers_;
}


void Observable::setObserver(const std::vector<Observer *> observer)
{
  observers_ = observer;
}


}