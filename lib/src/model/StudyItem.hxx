// StudyItem.hxx

#ifndef STUDYITEM_H
#define STUDYITEM_H

#include "PhysicalModelItem.hxx"
#include "CalculusItem.hxx"
#include "OTStudy.hxx"


namespace OTGUI {
class StudyItem : public Item
{
  Q_OBJECT

public:
  StudyItem(OTStudy * study);

  virtual ~StudyItem();

  void update(Observable * source, const std::string & message);

  void addCalculusItem(Calculus & calculus, CalculusItem * item);

  void setData(const QVariant & value, int role);
  OTStudy * getStudy();
  QString dumpStudy();

public slots:
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newCalculusItemCreated(CalculusItem*);

private:
  OTStudy * study_;
};
}
#endif