// OutputTableModel.hxx

#ifndef OUTPUTTABLEMODEL_H
#define OUTPUTTABLEMODEL_H

#include "Output.hxx"

#include <QAbstractTableModel>
#include <QModelIndex>

namespace OTGUI {
class OutputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  OutputTableModel();
  OutputTableModel(OutputCollection outputs);

  virtual ~OutputTableModel();

  OutputTableModel(const OutputTableModel & model);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  void removeLine(const QModelIndex & index);
  OutputCollection getData();
  bool isValid();

public slots:
    void addLine();
    void addLine(Output output);
private:
  OutputCollection data_;
  bool validity_;
};
}
#endif