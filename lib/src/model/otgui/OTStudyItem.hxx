// OTStudyItem.hxx

#ifndef OTSTUDYITEM_H
#define OTSTUDYITEM_H

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "otgui/LimitStateItem.hxx"
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

  void addDeterministicAnalysisItem(Analysis & analysis, AnalysisItem * item);
  void addProbabilisticAnalysisItem(Analysis & analysis, AnalysisItem * item);

  void setData(const QVariant & value, int role);
  OTStudy * getOTStudy();
  QString dumpOTStudy();

public slots:
  void updatePhysicalModel(const PhysicalModel & physicalModel);
  void updateAnalysis(const Analysis & analysis);
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newProbabilisticModelItemCreated(ProbabilisticModelItem*);
  void newDesignOfExperimentItemCreated(DesignOfExperimentItem*);
  void newAnalysisItemCreated(AnalysisItem*);
  void newLimitStateItemCreated(LimitStateItem*);

private:
  OTStudy * otStudy_;
};
}
#endif