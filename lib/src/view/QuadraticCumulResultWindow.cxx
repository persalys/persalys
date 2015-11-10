// QuadraticCumulResultWindow.cxx

#include "otgui/QuadraticCumulResultWindow.hxx"

#include "otgui/QuadraticCumulAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

QuadraticCumulResultWindow::QuadraticCumulResultWindow(CentralTendencyItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<QuadraticCumulAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void QuadraticCumulResultWindow::buildInterface()
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
  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
    items<<QString::fromStdString(result_.getOutputNames()[i]);
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBox_);

  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * label = new QLabel(tr("First order mean"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanFirstOrderLabel_ = new QLabel;
  grid->addWidget(meanFirstOrderLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Second order mean"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanSecondOrderLabel_ = new QLabel;
  grid->addWidget(meanSecondOrderLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Standard deviation"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  stdLabel_ = new QLabel;
  grid->addWidget(stdLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Variance"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  varianceLabel_ = new QLabel;
  grid->addWidget(varianceLabel_, gridRow, 1);

  grid->setRowStretch(++gridRow, 1);
  tabLayout->addLayout(grid);

  updateLabelsText(0);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void QuadraticCumulResultWindow::updateLabelsText(int indexOutput)
{
  // first order mean
  meanFirstOrderLabel_->setText(QString::fromStdString((OSS()<<result_.getMeanFirstOrder()[indexOutput]).str()));
  
  // second order mean
  meanSecondOrderLabel_->setText(QString::fromStdString((OSS()<<result_.getMeanSecondOrder()[indexOutput]).str()));

  // standard deviation
  stdLabel_->setText(QString::fromStdString((OSS()<<result_.getStandardDeviation()[indexOutput]).str()));

  // variance
  varianceLabel_->setText(QString::fromStdString((OSS()<<result_.getVariance()[indexOutput]).str()));
}

}