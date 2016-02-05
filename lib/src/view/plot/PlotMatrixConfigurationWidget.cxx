#include "otgui/PlotMatrixConfigurationWidget.hxx"
#include "otgui/ListWidgetWithCheckBox.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>

namespace OTGUI {

PlotMatrixConfigurationWidget::PlotMatrixConfigurationWidget(PlotMatrixWidget * plotMatrix)
  : QWidget()
  , plotMatrix_(plotMatrix)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, rowGrid, 0, 1, 1);
  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  label = new QLabel(tr("Colums"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  QComboBox * inputsComboBox = new QComboBox;
  ListWidgetWithCheckBox * listWidget = new ListWidgetWithCheckBox("-- Select variables --", plotMatrix_->getInputNames());
  connect(listWidget, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setInputsToDisplay(QStringList)));
  inputsComboBox->setModel(listWidget->model());
  inputsComboBox->setView(listWidget);
  mainGridLayout->addWidget(inputsComboBox, rowGrid, 1, 1, 1);

  label = new QLabel(tr("Rows"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  QComboBox * outputsComboBox = new QComboBox;
  listWidget = new ListWidgetWithCheckBox("-- Select variables --", plotMatrix_->getOutputNames());
  connect(listWidget, SIGNAL(checkedItemsChanged(QStringList)), plotMatrix_, SLOT(setOutputsToDisplay(QStringList)));
  outputsComboBox->setModel(listWidget->model());
  outputsComboBox->setView(listWidget);
  mainGridLayout->addWidget(outputsComboBox, rowGrid, 1, 1, 1);

  // Bottom layout
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  mainGridLayout->setRowStretch(++rowGrid, 1);

  updateLineEdits();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
}


void PlotMatrixConfigurationWidget::updateLineEdits()
{
  titleLineEdit_->blockSignals(true);

  titleLineEdit_->setText(plotMatrix_->getTitle());

  titleLineEdit_->blockSignals(false);
}


void PlotMatrixConfigurationWidget::updateTitle()
{
  plotMatrix_->setTitle(titleLineEdit_->text());
}


void PlotMatrixConfigurationWidget::exportPlot()
{
  plotMatrix_->exportPlot();
}
}