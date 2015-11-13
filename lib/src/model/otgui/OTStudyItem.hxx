// OTStudyItem.hxx

#ifndef OTSTUDYITEM_H
#define OTSTUDYITEM_H

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/OTStudy.hxx"

namespace OTGUI {
class OTStudyItem : public ObserverItem
{
  Q_OBJECT

public:
  OTStudyItem(OTStudy * otStudy);

  virtual ~OTStudyItem();

  void update(Observable * source, const std::string & message);

  void addAnalysisItem(Analysis & analysis, AnalysisItem * item, bool deterministic=true);

  void setData(const QVariant & value, int role);
  OTStudy * getOTStudy();
  QString dumpOTStudy();

public slots:
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newProbabilisticModelItemCreated(ProbabilisticModelItem*);
  void newAnalysisItemCreated(AnalysisItem*);

private:
  OTStudy * otStudy_;
};
}
#endif