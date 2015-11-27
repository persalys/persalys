// MonteCarloReliabilityResultWindow.cxx

#include "otgui/MonteCarloReliabilityResultWindow.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>

using namespace OT;

namespace OTGUI {

MonteCarloReliabilityResultWindow::MonteCarloReliabilityResultWindow(ReliabilityAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void MonteCarloReliabilityResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputName = new QLabel(result_.getEvent().getDescription()[0].c_str());
  headLayout->addWidget(outputName);
  headLayout->addStretch();
  
  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuLabel->setStyleSheet("font: bold 14px;");
  headLayout->addWidget(nbSimuLabel);
  nbSimuLabel = new QLabel(QString::number(result_.getOuterSampling()));
  headLayout->addWidget(nbSimuLabel);

  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * label = new QLabel(tr("Pf"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);

  double pfCILowerBound = std::max(0.0, result_.getProbabilityEstimate() - 0.5 * result_.getConfidenceLength());
  double pfCIUpperBound = std::min(1.0, result_.getProbabilityEstimate() + 0.5 * result_.getConfidenceLength());
  QString labelText = QString::number(result_.getProbabilityEstimate());
  labelText += "\n CI = [" + QString::number(pfCILowerBound) + ", ";
  labelText += QString::number(pfCIUpperBound) + "] at 95%";
  label = new QLabel(labelText);
  grid->addWidget(label, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("CV Pf"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);

  label = new QLabel(QString::number(result_.getCoefficientOfVariation()));
  grid->addWidget(label, gridRow, 1, Qt::AlignTop);

  grid->setRowStretch(++gridRow, 1);
  tabLayout->addLayout(grid);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}
}