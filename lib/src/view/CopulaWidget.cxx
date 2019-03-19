//                                               -*- C++ -*-
/**
 *  @brief QWidget to define copula
 *
 *  Copyright 2015-2019 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/CopulaWidget.hxx"

#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/TranslationManager.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/CorrelationTableModel.hxx"
#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/DocumentationToolButton.hxx"
#include "otgui/FileTools.hxx"

#include <QSplitter>
#include <QScrollArea>
#include <QDesktopServices>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

CopulaWidget::CopulaWidget(const PhysicalModel &model, const Copula &copula, QWidget *parent)
  : QWidget(parent)
  , physicalModel_(model)
  , copula_(copula)
  , parameterLayout_(0)
  , paramEditor_(0)
  , paramValueEdit_(0)
  , listPlot_()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QScrollArea * rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  QFrame * rightFrame = new QFrame;
  QVBoxLayout * rightFrameLayout = new QVBoxLayout(rightFrame);

  QSplitter * vSplitter = new QSplitter(Qt::Vertical);
  vSplitter->setStyleSheet("QSplitter::handle:vertical {height: 2px; background: lightGray;}");

  // -- plots creation
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  ResizableStackedWidget * plotStackedWidget = new ResizableStackedWidget;
  const QStringList variablesNames(QtOT::DescriptionToStringList(copula_.getDescription()));

  // create a PDF and a CDF for each pair of variables
  for (UnsignedInteger i = 0; i < copula_.getDimension(); ++i)
  {
    for (UnsignedInteger j = 0; j < copula_.getDimension(); ++j)
    {
      if (i != j)
      {
        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        PlotWidget * pdfPlot = new PlotWidget(tr("copulaPDF"), true);
        pdfPlot->plotContour(copula_.getMarginal(marginals));
        pdfPlot->setTitle(tr("PDF"));
        pdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        pdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        plotStackedWidget->addWidget(pdfPlot);
        listPlot_.append(pdfPlot);

        // cdf
        PlotWidget * cdfPlot = new PlotWidget(tr("copulaCDF"), true);
        cdfPlot->plotContour(copula_.getMarginal(marginals), false);
        cdfPlot->setTitle(tr("CDF"));
        cdfPlot->setAxisTitle(QwtPlot::xBottom, variablesNames[i]);
        cdfPlot->setAxisTitle(QwtPlot::yLeft, variablesNames[j]);
        plotStackedWidget->addWidget(cdfPlot);
        listPlot_.append(cdfPlot);
      }
    }
  }

  // -- GraphConfigurationWidget
  GraphConfigurationWidget * plotSettingWidget = new GraphConfigurationWidget(listPlot_,
      variablesNames,
      QStringList(),
      GraphConfigurationWidget::Copula,
      this);
  plotWidget->setDockWidget(plotSettingWidget);
  connect(plotSettingWidget, SIGNAL(currentPlotChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));

  plotWidgetLayout->addWidget(plotStackedWidget);

  vSplitter->addWidget(plotWidget);

  //  parameters
  QWidget * subWidget = new QWidget;
  parameterLayout_ = new QVBoxLayout(subWidget);

  // button to open the OT documentation
  DocumentationToolButton * infoButton = new DocumentationToolButton("", FileTools::docOT);
  connect(infoButton, SIGNAL(clicked()), this, SLOT(openDocUrl()));
  parameterLayout_->addWidget(infoButton);

  parameterLayout_->addStretch(1);

  vSplitter->addWidget(subWidget);
  rightFrameLayout->addWidget(vSplitter);

  rightScrollArea->setWidget(rightFrame);

  mainLayout->addWidget(rightScrollArea);
  updateParameters();
}


void CopulaWidget::updateParameters()
{
  // update parameters widgets
  if (paramEditor_)
  {
    parameterLayout_->removeWidget(paramEditor_);
    paramEditor_->disconnect();
    paramEditor_->deleteLater();
    paramEditor_ = 0;
  }

  //  parameters
  paramEditor_ = new QGroupBox(this);
  QGridLayout * groupBoxLayout = new QGridLayout(paramEditor_);

  // ---- if Normal copula : show spearman correlation table
  if (copula_.getImplementation()->getClassName() == "NormalCopula")
  {
    paramEditor_->setTitle(tr("Spearman's rank"));

    // correlation table view
    CopyableTableView * corrTableView = new CopyableTableView;
    corrTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    SpinBoxDelegate * corrDelegate = new SpinBoxDelegate(corrTableView);
    corrDelegate->setSpinBoxType(SpinBoxDelegate::correlation);
    corrTableView->setItemDelegate(corrDelegate);

    CorrelationTableModel * corrTableModel = new CorrelationTableModel(physicalModel_, copula_, corrTableView);
    corrTableView->setModel(corrTableModel);
    groupBoxLayout->addWidget(corrTableView, 0, 0);
    connect(corrTableModel, SIGNAL(dataUpdated(OT::Copula)), this, SLOT(updateCopulaFromCorrTable(OT::Copula)));
    connect(corrTableModel, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(emitErrorMessage(QString)));

    corrTableView->resizeColumnsToContents();
  }
  // ---- if NOT Normal copula : show the copula parameter value
  else
  {
    paramEditor_->setTitle(tr("Parameter"));
    String parameterName(copula_.getParameterDescription()[0]);
    QLabel * paramValueLabel_ = new QLabel(TranslationManager::GetTranslatedDistributionParameterName(parameterName));
    groupBoxLayout->addWidget(paramValueLabel_, 0, 0);

    paramValueEdit_ = new ValueLineEdit(copula_.getParameter()[0], paramEditor_);
    connect(paramValueEdit_, SIGNAL(editingFinished()), this, SLOT(updateCopulaFromLineEdit()));
    groupBoxLayout->addWidget(paramValueEdit_, 0, 1);
  }
  parameterLayout_->insertWidget(1, paramEditor_);
}


void CopulaWidget::updatePlots()
{
  for (UnsignedInteger i = 0; i < copula_.getDimension(); ++i)
  {
    int ind = 0;
    for (UnsignedInteger j = 0; j < copula_.getDimension(); ++j)
    {
      if (i != j)
      {
        Indices marginals(2);
        marginals[0] = i;
        marginals[1] = j;

        //pdf
        PlotWidget * pdfPlot = listPlot_[2 * (i * (copula_.getDimension() - 1) + ind)];
        pdfPlot->clear();
        pdfPlot->plotContour(copula_.getMarginal(marginals));

        // cdf
        PlotWidget * cdfPlot = listPlot_[2 * (i * (copula_.getDimension() - 1) + ind) + 1];
        cdfPlot->clear();
        cdfPlot->plotContour(copula_.getMarginal(marginals), false);
        ++ind;
      }
    }
  }
}


void CopulaWidget::setCopula(const Copula &copula)
{
  Q_ASSERT(copula.getDimension() == copula.getDimension());
  copula_ = copula;
  updateParameters();
  updatePlots();
}


void CopulaWidget::updateCopulaFromLineEdit()
{
  // store the description because the method setParameter overwrite the description
  const Description copulaDescription(copula_.getDescription());

  const double oldValue = copula_.getParameter()[0];
  try
  {
    double value = paramValueEdit_->value();
    if (value == oldValue)
      return;
    copula_.setParameter(Point(1, value));
    physicalModel_.blockNotification("ProbabilisticModel");
    physicalModel_.setCopula(copulaDescription, copula_);
    physicalModel_.blockNotification("");
    updatePlots();
  }
  catch (std::exception &ex)
  {
    emit emitErrorMessage(ex.what());
    paramValueEdit_->setValue(oldValue);
  }
  copula_.setDescription(copulaDescription);
}


void CopulaWidget::updateCopulaFromCorrTable(const Copula &copula)
{
  copula_ = copula;
  updatePlots();
}


void CopulaWidget::openDocUrl()
{
  const String copulaName = copula_.getImplementation()->getClassName();

  // open url
  const QString link = FileTools::OpenTURNSUrlLink + "user_manual/_generated/openturns." + QString(copulaName.c_str()) + ".html";
  QDesktopServices::openUrl(QUrl(link));
}
}
