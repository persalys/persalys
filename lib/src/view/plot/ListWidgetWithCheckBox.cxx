#include "otgui/ListWidgetWithCheckBox.hxx"

#include <QCheckBox>

namespace OTGUI {

ListWidgetWithCheckBox::ListWidgetWithCheckBox(QString title, QStringList itemNames, QWidget * parent)
  : QListWidget(parent)
  , itemNames_(itemNames)
  , checkedItemsNames_(itemNames)
{ 
  setViewMode(QListWidget::ListMode);
  setSelectionMode(QAbstractItemView::NoSelection);

  // first item
  QListWidgetItem * item = new QListWidgetItem;
  item->setData(Qt::DisplayRole, title);
  insertItem(model()->rowCount(), item);

  // other items associated to a checkBox
  for (int i=0; i<itemNames_.size(); ++i) 
  { 
    item = new QListWidgetItem;
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    insertItem(model()->rowCount(), item);

    QCheckBox * box = new QCheckBox(itemNames_[i]);
    box->setCheckState(Qt::Checked);
    setItemWidget(item, box);

    connect(box, SIGNAL(stateChanged(int)), this, SLOT(checkStateChanged(int)));
  } 
}


void ListWidgetWithCheckBox::checkStateChanged(int state)
{
  QCheckBox * box = qobject_cast<QCheckBox *>(sender());

  if (state == Qt::Checked)
  {
    if (checkedItemsNames_.contains(box->text()))
      return;
    else
      checkedItemsNames_.append(box->text());
  }
  else
  {
    if (!checkedItemsNames_.contains(box->text()))
      return;
    else
      checkedItemsNames_.removeAt(checkedItemsNames_.indexOf(box->text()));
  }
  emit checkedItemsChanged(checkedItemsNames_);
}
}