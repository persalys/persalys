// HorizontalHeaderViewWithCombobox.cxx

#include "otgui/HorizontalHeaderViewWithCombobox.hxx"

namespace OTGUI {

HorizontalHeaderViewWithCombobox::HorizontalHeaderViewWithCombobox(QStringList comboBoxItems,
                                                                   QVector<int> columns, QWidget * parent)
  : QHeaderView(Qt::Horizontal, parent)
  , comboBoxItems_(comboBoxItems)
  , columnsWithComboBox_(columns)
{
  connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleSectionResized(int)));
  connect(this, SIGNAL(sectionMoved(int, int, int)), this, SLOT(handleSectionMoved(int, int, int)));
  setMovable(true);
}


void HorizontalHeaderViewWithCombobox::showEvent(QShowEvent * e)
{
  for (int i=0; i<columnsWithComboBox_.size(); ++i)
  {
    int j = columnsWithComboBox_[i];
    if (!boxes_[j])
    {
      QComboBox * box = new QComboBox(this);
      box->addItems(comboBoxItems_);
      box->setCurrentIndex(box->findText(model()->headerData(j, Qt::Horizontal).toString()));
      connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(setHeaderData(int)));
      boxes_[j] = box;
    }
    boxes_[j]->setGeometry(sectionViewportPosition(j), 0, sectionSize(j) - 1, height());
    boxes_[j]->show();
  }
  QHeaderView::showEvent(e);
}


void HorizontalHeaderViewWithCombobox::handleSectionResized(int i)
{
  for (int i=0; i<columnsWithComboBox_.size(); ++i)
  {
    int logical = logicalIndex(columnsWithComboBox_[i]);
    boxes_[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 1, height());
  }
}

void HorizontalHeaderViewWithCombobox::handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex)
{
  for (int i=qMin(oldVisualIndex, newVisualIndex); i<count(); ++i)
  {
    int logical = logicalIndex(columnsWithComboBox_[i]);
    boxes_[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 1, height());
  }
}


void HorizontalHeaderViewWithCombobox::setHeaderData(int i)
{
  QComboBox * box = qobject_cast<QComboBox *>(sender());

  QMap<int, QComboBox*>::const_iterator it;
  for (it = boxes_.begin(); it != boxes_.end(); ++it)
    if (it.value() == box)
      break;

  if (it == boxes_.end())
    throw;
  model()->setHeaderData(it.key(), Qt::Horizontal, box->currentText(), Qt::DisplayRole);
}


void HorizontalHeaderViewWithCombobox::scrollContentsBy(int dx, int dy)
{
  QHeaderView::scrollContentsBy(dx, dy);
  if (dx != 0)
    fixComboPositions();
}


void HorizontalHeaderViewWithCombobox::fixComboPositions()
{
  for (int i=0; i<columnsWithComboBox_.size(); ++i)
  {
    int j = columnsWithComboBox_[i];
    boxes_[j]->setGeometry(sectionViewportPosition(j), 0, sectionSize(j) - 1, height());
  }
}
}