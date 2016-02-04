// TaylorExpansionsMomentsResultWindow.cxx

#include "otgui/TaylorExpansionsMomentsResultWindow.hxx"

#include "otgui/TaylorExpansionsMomentsAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

TaylorExpansionsMomentsResultWindow::TaylorExpansionsMomentsResultWindow(CentralTendencyItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<TaylorExpansionsMomentsAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void TaylorExpansionsMomentsResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBox_ = new QComboBox;
  QStringList items = QStringList();
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    items << result_.getOutputNames()[i].c_str();
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBox_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * label = new QLabel(tr("First order mean"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanFirstOrderLabel_ = new QLabel;
  grid->addWidget(meanFirstOrderLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Second order mean"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanSecondOrderLabel_ = new QLabel;
  grid->addWidget(meanSecondOrderLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Standard deviation"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  stdLabel_ = new QLabel;
  grid->addWidget(stdLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Variance"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  varianceLabel_ = new QLabel;
  grid->addWidget(varianceLabel_, gridRow, 1);

  grid->setRowStretch(++gridRow, 1);
  grid->setColumnStretch(1, 1);
  tabLayout->addLayout(grid);

  updateLabelsText(0);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void TaylorExpansionsMomentsResultWindow::updateLabelsText(int indexOutput)
{
  // first order mean
  meanFirstOrderLabel_->setText(QString::number(result_.getMeanFirstOrder()[indexOutput]));

  // second order mean
  meanSecondOrderLabel_->setText(QString::number(result_.getMeanSecondOrder()[indexOutput]));

  // standard deviation
  stdLabel_->setText(QString::number(result_.getStandardDeviation()[indexOutput]));

  // variance
  varianceLabel_->setText(QString::number(result_.getVariance()[indexOutput]));
}
}