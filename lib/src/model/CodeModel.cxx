#include "otgui/CodeModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CodeModel::CodeModel(const PhysicalModel & physicalModel)
  : QAbstractTableModel()
  , physicalModel_(physicalModel)
{
}


CodeModel::~CodeModel()
{
}


int CodeModel::columnCount(const QModelIndex & parent) const
{
  return 1;
}


int CodeModel::rowCount(const QModelIndex & parent) const
{
  return 1;
}

QVariant CodeModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    PythonPhysicalModel *pyModel =  dynamic_cast<PythonPhysicalModel*>(&*physicalModel_.getImplementation().get());
    return pyModel? pyModel->getCode().c_str() : "";
  }
  return QVariant();
}


bool CodeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    physicalModel_.blockNotification(true, "codeChanged");
    dynamic_cast<PythonPhysicalModel*>(&*physicalModel_.getImplementation().get())->setCode(value.toString().toStdString());
    physicalModel_.blockNotification(false);
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags CodeModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


}
