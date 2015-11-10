// StudyItem.hxx

#ifndef STUDYITEM_H
#define STUDYITEM_H

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/OTStudy.hxx"

namespace OTGUI {
class StudyItem : public Item
{
  Q_OBJECT

public:
  StudyItem(OTStudy * study);

  virtual ~StudyItem();

  void update(Observable * source, const std::string & message);

  void addAnalysisItem(Analysis & analysis, AnalysisItem * item);

  void setData(const QVariant & value, int role);
  OTStudy * getStudy();
  QString dumpStudy();

public slots:
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newAnalysisItemCreated(AnalysisItem*);

private:
  OTStudy * study_;
};
}
#endif