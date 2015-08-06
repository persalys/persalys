// StudyItem.hxx

#ifndef STUDYITEM_H
#define STUDYITEM_H

#include "PhysicalModelItem.hxx"
#include "ParametricCalculusItem.hxx"
#include "OTStudy.hxx"


namespace OTGUI {
class StudyItem : public Item
{
  Q_OBJECT

public:
  StudyItem(OTStudy * study);

  virtual ~StudyItem();

  void update(Observable * source, std::string message);

  void setData(const QVariant& value, int role);
  OTStudy * getStudy();
  QString dumpStudy();

public slots:
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newParametricCalculusItemCreated(ParametricCalculusItem*);
private:
  OTStudy * study_;
};
}
#endif