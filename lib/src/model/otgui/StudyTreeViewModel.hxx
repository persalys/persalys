// StudyTreeViewModel.hxx

#ifndef STUDYTREEVIEWMODEL_H
#define STUDYTREEVIEWMODEL_H

#include "otgui/OTStudyItem.hxx"

#include <QStandardItemModel>

namespace OTGUI {
class StudyTreeViewModel : public QStandardItemModel, public Observer
{
  Q_OBJECT

public:
  StudyTreeViewModel();

  virtual ~StudyTreeViewModel();

  void createNewOTStudy();
  void addOTStudyItem(OTStudy * otStudy);
  void addPhysicalModelItem(const QModelIndex & parentIndex);
  void addProbabilisticModelItem(const QModelIndex & parentIndex);
  void addLimitStateItem(const QModelIndex & parentIndex);

  OTStudyItem * getOTStudyItem(const QModelIndex & childIndex);

  virtual void update(Observable * source, const std::string & message);

public slots:
signals:
  void newPhysicalModelCreated(PhysicalModelItem*);
  void newProbabilisticModelCreated(ProbabilisticModelItem*);
  void newLimitStateCreated(LimitStateItem*);
  void newAnalysisCreated(AnalysisItem*);
};
}
#endif