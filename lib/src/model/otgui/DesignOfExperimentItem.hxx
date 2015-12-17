// DesignOfExperimentItem.hxx

#ifndef DESIGNOFEXPERIMENTITEM_H
#define DESIGNOFEXPERIMENTITEM_H

#include "otgui/ObserverItem.hxx"
#include "otgui/DesignOfExperiment.hxx"

namespace OTGUI {
class DesignOfExperimentItem : public ObserverItem
{
  Q_OBJECT

public:
  DesignOfExperimentItem(const DesignOfExperiment & designOfExperiment);

  virtual ~DesignOfExperimentItem();

  DesignOfExperiment getDesignOfExperiment() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const std::string & message);

private:
  DesignOfExperiment designOfExperiment_;
};
}
#endif