// ParametricAnalysisTableModel.hxx

#ifndef PARAMETRICANALYSISTABLEMODEL_H
#define PARAMETRICANALYSISTABLEMODEL_H

#include "otgui/Analysis.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class ParametricAnalysisTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ParametricAnalysisTableModel(const Analysis & analysis);

  virtual ~ParametricAnalysisTableModel();

  Analysis getAnalysis() const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void fillTable();

public slots:
signals:
  void dataChanged(Analysis&);

private:
  Analysis analysis_;
};
}
#endif