// CodeModel.hxx

#ifndef OTGUI_CODEMODEL_H
#define OTGUI_CODEMODEL_H

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class CodeModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  CodeModel(const PhysicalModel & physicalModel);

  virtual ~CodeModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;

private:
  PhysicalModel physicalModel_;
};
}
#endif