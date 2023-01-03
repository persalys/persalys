//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a coupling physical model
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_COUPLINGMODELWINDOW_HXX
#define PERSALYS_COUPLINGMODELWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/PhysicalModelItem.hxx"
#include "persalys/CouplingPhysicalModel.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/CopyableTableView.hxx"

#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QMessageBox>
#include <QTableWidget>
#include <QFileDialog>
#include <QLineEdit>

namespace PERSALYS
{

class PERSALYS_VIEW_API InTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InTableModel(CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent);

  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  int columnCount(const QModelIndex & parent = QModelIndex()) const override;

  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;

private:
  CouplingInputFile getInputFile() const;
  void updateModel(const CouplingInputFile &file);

public slots:
  void updateData();
  void addLine();
  void removeLine();

private:
  CouplingPhysicalModel * model_ = 0;
  int indStep_;
  int indFile_;
};

class PERSALYS_VIEW_API OutTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  OutTableModel(CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent);

  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  int columnCount(const QModelIndex & parent = QModelIndex()) const override;

  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;

private:
  CouplingOutputFile getOutputFile() const;
  void updateModel(const CouplingOutputFile &file);

public slots:
  void updateData();
  void addLine();
  void removeLine();

private:
  CouplingPhysicalModel * model_ = 0;
  int indStep_;
  int indFile_;
};


class PERSALYS_VIEW_API FilePathWidget : public QWidget
{
  Q_OBJECT

public:
  FilePathWidget(const QString &path="", QFileDialog::FileMode mode=QFileDialog::AnyFile, QWidget *parent=0)
    : QWidget(parent)
  {
    QHBoxLayout * hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 0, 0);

    edit_ = new QLineEdit(path);
    hLayout->addWidget(edit_);
    edit_->setValidator(new QRegularExpressionValidator(QRegularExpression("([^\r\n]*)")));
    connect(edit_, &QLineEdit::editingFinished, [=]() { emit pathChanged(edit_->text()); });

    QToolButton * button = new QToolButton;
    button->setText("...");
    hLayout->addWidget(button);

    connect(button, &QToolButton::clicked,
            [=]() {
	      QFileDialog dialog;
	      dialog.setFileMode(mode);
	      QString fileName;
	      if(mode == QFileDialog::Directory)
		fileName = dialog.getExistingDirectory(this, tr("Choose directory"), FileTools::GetCurrentDir());
	      else
		fileName = dialog.getOpenFileName(this, tr("Search file"), FileTools::GetCurrentDir());
	      if (fileName.isEmpty())
		return;
	      FileTools::SetCurrentDir(fileName);
	      edit_->setText(fileName);
	      emit pathChanged(fileName);
	    });
  }
  QString text()
  {
    return edit_->text();
  }
signals:
  void pathChanged(const QString&);
private:
    QLineEdit * edit_ = 0;
};

class PERSALYS_VIEW_API DynamicTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  DynamicTabWidget()
  {
    setTabsClosable(true);
    // Create button what must be placed in tabs row
    QToolButton * tb = new QToolButton;
    tb->setIcon(QIcon(":/images/list-add.png"));
    tb->setAutoRaise(true);
    connect(tb, SIGNAL(clicked(bool)), this, SIGNAL(newTabRequested()));
    // Add empty, not enabled tab to tabWidget
    QTabWidget::addTab(new QWidget(), QString());
    setTabEnabled(0, false);
    // Add tab button to current tab. Button will be enabled, but tab -- not
    tabBar()->setTabButton(0, QTabBar::RightSide, tb);
    connect(tabBar(), &QTabBar::tabCloseRequested, this, &DynamicTabWidget::tabCloseRequested);

    // override tab bar style sheet
    QString sheet(tabBar()->styleSheet());
    sheet += "QTabBar::tab:disabled {border: 1px solid #C4C4C3; padding: 2px; border-bottom: 0px; border-radius: 2px;}";
    tabBar()->setStyleSheet(sheet);
  }
  virtual int addTab(QWidget *page, const QString &label)
  {
    insertTab(count()-1, page, label);
    setCurrentIndex(count()-2);
    return count()-2;
  }
  virtual void clear()
  {
    while (count()-1)
      removeTab(0);
  }
  void tabCloseRequested(int index)
  {
    int ret = QMessageBox::question(this, tr("Remove"), tr("Do you really want to remove the variables?"));
    if (ret == QMessageBox::Yes)
    {
      removeTab(index);
      setCurrentIndex(index-1);
      emit removeTabRequested(index);
      if (count() < 2)
        emit newTabRequested();
    }
  }
signals:
  void newTabRequested();
  void removeTabRequested(int);
};


class PERSALYS_VIEW_API AddRemoveWidget : public QWidget
{
  Q_OBJECT

public:
  AddRemoveWidget()
  {
    QPushButton * addButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    connect(addButton, SIGNAL(clicked(bool)), this, SIGNAL(addRequested()));

    QPushButton * removeButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    connect(removeButton, SIGNAL(clicked(bool)), this, SIGNAL(removeRequested()));

    QHBoxLayout * buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(removeButton);
  }
signals:
  void addRequested();
  void removeRequested();
};


class PERSALYS_VIEW_API CouplingInputFileWidget : public QWidget
{
  Q_OBJECT

public:
  CouplingInputFileWidget(PhysicalModelItem *item, CouplingPhysicalModel * model, const int indStep, const int indFile, QWidget *parent = 0);
  int getIndFile() const { return indFile_; };
  QString readFile(QFileInfo & fname) const;
  void compareFiles(QString & s1, QString & s2) const;
signals:
  void variableListChanged();
private:
  int indStep_;
  int indFile_;
};


class PERSALYS_VIEW_API CouplingResourceFileWidget : public QWidget
{
  Q_OBJECT

public:
  CouplingResourceFileWidget(CouplingPhysicalModel * model, const int indStep, QWidget *parent = 0);
  void updateTable();

signals:
  void couplingResourceCollectionModified();
public slots:
  void editResource(QTableWidgetItem * newItem, bool isFile);
private:
  CouplingPhysicalModel * model_ = 0;
  int indStep_;
  QTableWidget * tableWidget_ = 0;
};


class PERSALYS_VIEW_API CouplingOutputFileWidget : public QWidget
{
  Q_OBJECT

public:
  CouplingOutputFileWidget(PhysicalModelItem *item, CouplingPhysicalModel * model, const int indStep, const int indFile, QWidget *parent = 0);
signals:
  void variableListChanged();
};


class PERSALYS_VIEW_API CouplingStepWidget : public QWidget
{
  Q_OBJECT

public:
  CouplingStepWidget(PhysicalModelItem *item, CouplingPhysicalModel * model, const int indStep, QWidget *parent = 0);
public slots:
  void updateInputFileWidgets(PhysicalModelItem *item);
signals:
  void variableListChanged();
  void updateStepRequested();

private:
  CouplingPhysicalModel * model_ = 0;
  int indStep_ = 0;
  DynamicTabWidget * inTabWidget_ = 0;
  CouplingResourceFileWidget * resourceFileWidget_ = 0;
};


class PERSALYS_VIEW_API CouplingModelWindow : public SubWindow
{
  Q_OBJECT

public:
  CouplingModelWindow(PhysicalModelItem *item, QWidget * parent = 0);
public slots:
  void updateStepTabWidget(PhysicalModelItem *item);
  void evaluateOutputs();
signals:
  void variableListChanged();
private:
  CouplingPhysicalModel * model_ = 0;
  DynamicTabWidget * stepTabWidget_ = 0;
  TemporaryLabel * errorMessageLabel_ = 0;
};

class PERSALYS_VIEW_API CouplingSummaryWidget : public QTabWidget
{
  Q_OBJECT

public :
  CouplingSummaryWidget(PhysicalModelItem * item);

protected slots:
  void showEvent(QShowEvent *event);
signals:
  void evaluationRequested();
private:
  PhysicalModel model_;
  CopyableTableView * inputTableView_;
  CopyableTableView * outputTableView_;
};

class PERSALYS_VIEW_API PythonCodeModel : public QAbstractTableModel
{
  Q_OBJECT

public :
  PythonCodeModel(PhysicalModelItem * item, int indStep, QWidget * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;

public slots:
  void updateData();

signals:
  void dataChanged();
private :
  PhysicalModel model_;
  int indStep_;
};

class PERSALYS_VIEW_API PythonCodeWidget : public QWidget
{
  Q_OBJECT

public :
  PythonCodeWidget(PhysicalModelItem * item, const int indStep, QWidget *parent);
signals :
  void codeChanged();
};
}
#endif
