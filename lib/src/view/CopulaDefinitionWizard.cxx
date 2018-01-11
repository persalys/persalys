//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a copula
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/CopulaDefinitionWizard.hxx"

#include "otgui/VariablesSelectionTableModel.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/TranslationManager.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/PlotWidget.hxx"

#include <openturns/NormalCopula.hxx>

#include <QVBoxLayout>
#include <QTableView>
#include <QGroupBox>
#include <QRadioButton>

using namespace OT;

namespace OTGUI
{

CopulaDefinitionWizard::CopulaDefinitionWizard(const PhysicalModel &model, QWidget *parent)
  : OTguiWizard(parent)
  , physicalModel_(model)
  , copulaVariables_(2)
  , pageValidity_(true)
{
  buildInterface();
}


void CopulaDefinitionWizard::buildInterface()
{
  setWindowTitle(tr("Copula definition"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QHBoxLayout * splitter = new QHBoxLayout;

  // table view
  QTableView * tableView = new QTableView;
  tableView->setSelectionMode(QAbstractItemView::NoSelection);
  tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tableView->verticalHeader()->hide();
  tableView->setShowGrid(false);

  // - model
  Collection<Copula> coll(physicalModel_.getComposedCopula().getCopulaCollection());
  Description independentVar;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() == "IndependentCopula")
      independentVar.add(coll[i].getDescription());
  }

  Interval::BoolCollection isVarChecked(independentVar.getSize());
  for (UnsignedInteger i = 0; i < 2; ++i)
  {
    isVarChecked[i] = true;
    copulaVariables_[i] = independentVar[i];
  }

  VariablesSelectionTableModel * tableModel = new VariablesSelectionTableModel(independentVar, isVarChecked, tableView);
  tableView->setModel(tableModel);

  connect(tableModel, SIGNAL(selectionChanged(OT::Description, OT::String)), this, SLOT(updateLineEdit(OT::Description, OT::String)));

  // - horizontal header
  CheckableHeaderView * headerView = new CheckableHeaderView;
  tableView->setHorizontalHeader(headerView);

  // - resize table
  int w = headerView->length();
  tableView->resizeColumnToContents(0);
  if (headerView->length() > w)
    w = headerView->length();
  int x1, y1, x2, y2;
  tableView->getContentsMargins(&x1, &y1, &x2, &y2);
  tableView->setFixedWidth(w + x1 + x2);
  headerView->resizeSection(0, w + x1 + x2);

  splitter->addWidget(tableView);

  // widget to choose the copula
  QWidget * rightWidget = new QWidget;
  QVBoxLayout * rightWidgetLayout = new QVBoxLayout(rightWidget);

  // copula
  copulaChoice_ = new QButtonGroup(this);

  QGroupBox * copulaGroupBox = new QGroupBox(tr("Copula"));
  QVBoxLayout * copulaGroupBoxLayout = new QVBoxLayout(copulaGroupBox);

  const QStringList availableCopulas = TranslationManager::GetAvailableCopulas();
  ResizableStackedWidget * plotStackedWidget = new ResizableStackedWidget;
  connect(copulaChoice_, SIGNAL(buttonClicked(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));

  for (int i = 0; i < availableCopulas.size(); ++i)
  {
    QRadioButton * radioButton = new QRadioButton(availableCopulas[i]);
    copulaGroupBoxLayout->addWidget(radioButton);
    copulaChoice_->addButton(radioButton, i);

    const String newCopulaName = TranslationManager::GetCopulaName(availableCopulas[i]);
    Copula copula;
    if (availableCopulas[i] == tr("Normal"))
    {
      radioButton->setChecked(true);
      CorrelationMatrix corrMatrix(2);
      corrMatrix(0, 1) = 0.5;
      copula = NormalCopula(corrMatrix);
    }
    else
    {
      radioButton->setToolTip(tr("Bivariate copula"));
      copula = DistributionDictionary::BuildCopulaFactory(newCopulaName).build();
    }

    PlotWidget * pdfPlot = new PlotWidget(tr("copulaPDF"), true);
    pdfPlot->plotContour(copula);
    String copulaName = newCopulaName;
    copulaName = copulaName.substr(0, copulaName.find("Copula"));
    const QString distName = TranslationManager::GetTranslatedCopulaName(copulaName);
    QwtText title(tr("PDF") + " " + tr("%1 copula").arg(distName));
    QFont titleFont(title.font());
    titleFont.setPointSize(10);
    title.setFont(titleFont);
    title.setRenderFlags(title.renderFlags() | Qt::TextWordWrap);
    pdfPlot->setTitle(title);

    pdfPlot->setAxisTitle(QwtPlot::xBottom, QwtText("u<sub>1</sub>", QwtText::RichText));
    pdfPlot->setAxisTitle(QwtPlot::yLeft, QwtText("u<sub>2</sub>", QwtText::RichText));

    plotStackedWidget->addWidget(pdfPlot);
    if (availableCopulas[i] == tr("Normal"))
      plotStackedWidget->setCurrentIndex(i);
  }

  rightWidgetLayout->addWidget(copulaGroupBox);
  rightWidgetLayout->addWidget(plotStackedWidget, 0, Qt::AlignHCenter);
  rightWidgetLayout->addStretch();

  splitter->addWidget(rightWidget);

  pageLayout->addLayout(splitter);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
}


void CopulaDefinitionWizard::updateLineEdit(const Description& checkedVariables, const String&)
{
  pageValidity_ = true;
  errorMessageLabel_->setText("");
  copulaVariables_ = checkedVariables;

  for (int i = 0; i < copulaChoice_->buttons().size(); ++i)
  {
    copulaChoice_->button(i)->setEnabled(true);
    if (checkedVariables.getSize() < 2 ||
       (checkedVariables.getSize() > 2 && copulaChoice_->button(i)->text() != tr("Normal")))
    {
      copulaChoice_->button(i)->setEnabled(false);
    }
    else if (checkedVariables.getSize() > 2 && copulaChoice_->button(i)->text() == tr("Normal"))
    {
      copulaChoice_->button(i)->setChecked(true);
    }
  }
  if (checkedVariables.getSize() < 2)
  {
    pageValidity_ = false;
    const QString errorMessage = tr("Select at least two variables to define a copula");
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
  }
}


bool CopulaDefinitionWizard::validateCurrentPage()
{
  return pageValidity_;
}


Copula CopulaDefinitionWizard::getCopula() const
{
  const String newCopulaName = TranslationManager::GetCopulaName(copulaChoice_->checkedButton()->text());
  Copula copula;
  if (copulaChoice_->checkedButton()->text() != tr("Normal"))
    copula = DistributionDictionary::BuildCopulaFactory(newCopulaName).build();
  else
    copula = NormalCopula(copulaVariables_.getSize());
  copula.setDescription(copulaVariables_);
  return copula;
}
}
