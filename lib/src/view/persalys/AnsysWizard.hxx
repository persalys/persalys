//                                               -*- C++ -*-
/**
 *  @brief Specific Wizard for the Ansys coupling model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#ifndef PERSALYS_ANSYSWIZARD_HXX
#define PERSALYS_ANSYSWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/AnsysParser.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/AnsysVariableTableModel.hxx"
#include "persalys/AnsysSystemTableModel.hxx"

#include <QLineEdit>
#include <QSpinBox>
#include <QTableView>

namespace PERSALYS
{
class PERSALYS_VIEW_API AnsysWizardVariablePage : public QWizardPage
{
  Q_OBJECT
public:
  friend class AnsysWizard;
  AnsysWizardVariablePage(QWidget *parent = nullptr);
  virtual bool validatePage();

public slots:
  void findExecutableFile();
  void findModelFile();

signals:
  void loadModelRequested();
  void executableFileFound();

private:
  TemporaryLabel * errorMessageLabel_;
  QLineEdit * executableLineEdit_;
  QLineEdit * modelFileLineEdit_;
  AnsysVariableTableModel * varModel_;
  QTableView * varTable_;
};

class PERSALYS_VIEW_API AnsysWizardSystemPage : public QWizardPage
{
  Q_OBJECT
public:
  friend class AnsysWizard;
  AnsysWizardSystemPage(QWidget *parent = nullptr);
  virtual bool validatePage();

private:
  TemporaryLabel * errorMessageLabel_;
  AnsysSystemTableModel * sysModel_;
  QTableView * sysTable_;
};

class PERSALYS_VIEW_API AnsysWizard : public Wizard
{
  Q_OBJECT
public:
  AnsysWizard(QWidget *parent = nullptr);

  void validateVariables();
  void validateSystems();

  QString getExecutableFileName() const;
  QString getModelFileName() const;
  AnsysParser * getParser() const;

public slots:
  void loadModel();

private:
  AnsysParser * parser_;
  AnsysWizardVariablePage * varPage_;
  AnsysWizardSystemPage * sysPage_;
};
}
#endif // PMS_ANSYSWIZARD_H
