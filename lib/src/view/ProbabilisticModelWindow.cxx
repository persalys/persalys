// ProbabilisticModelWindow.cxx

#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"

#include <QSplitter>
#include <QHeaderView>
#include <QScrollArea>

using namespace OT;

namespace OTGUI {

ProbabilisticModelWindow::ProbabilisticModelWindow(ProbabilisticModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , paramEditor_(0)
{
  buildInterface();
}


void ProbabilisticModelWindow::buildInterface()
{
  QTabWidget * rootTab = new QTabWidget;
  
  // First Tab: marginals
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  QSplitter * horizontalSplitter = new QSplitter;

  // Inputs
  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_.getInputs());
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));

  inputTableView_ = new QTableView;
  inputTableView_->setModel(inputTableModel_);
  ComboBoxDelegate * delegate = new ComboBoxDelegate;
  inputTableView_->setItemDelegateForColumn(1, delegate);
  inputTableView_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  for (int i = 0; i < inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));
  connect(inputTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  inputTableView_->selectRow(0);

  horizontalSplitter->addWidget(inputTableView_);

  // Distribution edition
  QScrollArea * rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  QFrame * rightFrame_ = new QFrame;
  QVBoxLayout * rightLayout_ = new QVBoxLayout(rightFrame_);

  //  PDF and CDF graphs
  QGridLayout * plotLayout = new QGridLayout;
  pdfPlot_ = new PlotWidget;
  plotLayout->addWidget(pdfPlot_->getPlotLabel(), 1, 0);
  cdfPlot_ = new PlotWidget;
  plotLayout->addWidget(cdfPlot_->getPlotLabel(), 1, 1);
  rightLayout_->addLayout(plotLayout);

  //  parameters
  parameterLayout_ = new QHBoxLayout;
  parameterLayout_->addWidget(new QWidget);
  parameterLayout_->addStretch();
  rightLayout_->addLayout(parameterLayout_);
  updateDistributionWidgets(inputTableView_->currentIndex());

  rightLayout_->addStretch();
  rightScrollArea->setWidget(rightFrame_);
  horizontalSplitter->addWidget(rightScrollArea);

  tabLayout->addWidget(horizontalSplitter);
  rootTab->addTab(tab, tr("Marginals"));
  
  setWidget(rootTab);
}


void ProbabilisticModelWindow::updateDistributionWidgets(const QModelIndex & index)
{
  if (!index.isValid())
    return;

  // update parameters widgets
  if (paramEditor_)
  {
    parameterLayout_->removeWidget(paramEditor_);
    paramEditor_->disconnect();
    paramEditor_->deleteLater();
    paramEditor_ = 0;
  }

  const NumericalPointWithDescription parameters = inputTableModel_->getParameters(index.row());
  int nbParameters = parameters.getDescription().getSize();
  if (nbParameters)
  {
    Description parametersName = parameters.getDescription();

    paramEditor_ = new QGroupBox(tr("Parameters"));
    QGridLayout * lay = new QGridLayout(paramEditor_);
    for (int i=0; i<nbParameters; ++i)
    {
      parameterValuesLabel_[i] = new QLabel(QString(parametersName[i].c_str()));
      parameterValuesEdit_[i] = new QLineEdit(QString::number(parameters[i]));
      connect(parameterValuesEdit_[i], SIGNAL(textChanged(QString)), this, SLOT(updateDistribution()));
      parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
      lay->addWidget(parameterValuesLabel_[i], i, 0);
      lay->addWidget(parameterValuesEdit_[i], i, 1);
    }
    parameterLayout_->insertWidget(0, paramEditor_);
  }

  // update plots
  pdfPlot_->clear();
  pdfPlot_->plotPDFCurve(inputTableModel_->getData()[index.row()].getDistribution());
  cdfPlot_->clear();
  cdfPlot_->plotCDFCurve(inputTableModel_->getData()[index.row()].getDistribution());
}


void ProbabilisticModelWindow::updateDistribution()
{
  QModelIndex index = inputTableView_->selectionModel()->currentIndex();
  NumericalPoint parameters(2);
  parameters[0] = parameterValuesEdit_[0]->text().toDouble();
  parameters[1] = parameterValuesEdit_[1]->text().toDouble();
  inputTableModel_->updateDistributionParameters(index, parameters);
}
}
