// StudyTreeViewModel.hxx

#ifndef STUDYTREEVIEWMODEL_H
#define STUDYTREEVIEWMODEL_H

#include "otgui/OTStudyItem.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/ParametricAnalysisItem.hxx"

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

  virtual void update(Observable * source, const std::string & message);

public slots:
signals:
  void newPhysicalModelCreated(PhysicalModelItem*);
  void newAnalysisCreated(AnalysisItem*);
};
}
#endif