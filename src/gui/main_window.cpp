#include "gui/main_window.h"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessagebox>
#include <QScrollArea>
#include <QString>
#include <QTabWidget>
#include <QtDebug>  // TODO: Remove when done.

#include <memory>
#include <vector>

#include "gui/class_spectra_view.h"
#include "gui/class_spectrum_row.h"
#include "gui/export_view.h"
#include "gui/image_layout_view.h"
#include "hsi/image_layout.h"
#include "gui/layout_blend_view.h"
#include "hsi/project_loader.h"
#include "hsi/spectrum.h"
#include "util/util.h"

namespace hsi_data_generator {
namespace {

// The default title. The window_counter is used to uniquely label multiple new
// windows.
static const QString kDefaultWindowTitle = "New Project";
static int window_counter = 0;

// File menu items:
static const QString kFileMenuText = "File";

static const QString kNewActionText = "New";
static const QString kNewActionTip = "Opens a new project window";

static const QString kOpenActionText = "Open";
static const QString kOpenActionTip = "Open an existing workflow";

static const QString kResetActionText = "Reset";
static const QString kResetActionTip = "Reset the current workflow";

static const QString kSaveActionText = "Save";
static const QString kSaveActionTip = "Save your current workflow to a file";

static const QString kClassSpectraViewString = "Class Spectra";
static const QString kClassSpectraViewToolTip =
    "Create and edit a spectral dictionary used to build the final image.";
static const QString kImageLayoutViewString = "Image Layout";
static const QString kImageLayoutViewToolTip =
    "Design and arrange the 2D spatial layout of the spectra.";
static const QString kLayoutBlendViewString = "Layout Blending";
static const QString kLayoutBlendViewToolTip =
    "Define how neighboring spectra mix together.";
static const QString kExportViewString = "Export HSI";
static const QString kExportViewToolTip =
    "Set noise and blur properties, and export the HSI data to a file.";

// Dialog strings:
static const QString kResetWarningDialogTitle = "Reset Project?";
static const QString kResetWarningDialogMessage =
    QString("Are you sure you want to reset this project? ") +
    QString("All changed will be erased. This action cannot be undone.");

static const QString kSaveProjectDialogTitle = "Save Project";
static const QString kSaveProjectErrorDialogTitle = "Save Project Error";
static const QString kOpenProjectDialogTitle = "Open Project";
static const QString kOpenProjectErrorDialogTitle = "Open Project Error";

// File operation filters:
static const QString kXMLFileFilter = "XML (*.xml)";

// Default values for the GUI widgets:
constexpr int kDefaultNumberOfBands = 100;
constexpr int kDefaultImageLayoutWidth = 500;
constexpr int kDefaultImageLayoutHeight = 500;

}  // namespace

MainWindow::MainWindow() {
  QString window_title = kDefaultWindowTitle;
  if (window_counter > 0) {
    window_title += " (" + QString::number(window_counter) + ")";
  }
  setWindowTitle(window_title);
  window_counter++;

  // Create a file menu:
  QMenu* file_menu = menuBar()->addMenu(kFileMenuText);

  // The "new" action menu item:
  QAction* new_action = new QAction(kNewActionText, this);
  new_action->setStatusTip(kNewActionTip);
  new_action->setShortcuts(QKeySequence::New);
  file_menu->addAction(new_action);
  connect(new_action, SIGNAL(triggered()), this, SLOT(NewActionCalled()));

  // The "open" action menu item:
  QAction* open_action = new QAction(kOpenActionText, this);
  open_action->setStatusTip(kOpenActionTip);
  open_action->setShortcuts(QKeySequence::Open);
  file_menu->addAction(open_action);
  connect(open_action, SIGNAL(triggered()), this, SLOT(OpenActionCalled()));

  // The "reset" action menu item:
  QAction* reset_action = new QAction(kResetActionText, this);
  reset_action->setStatusTip(kResetActionTip);
  file_menu->addAction(reset_action);
  connect(reset_action, SIGNAL(triggered()), this, SLOT(ResetActionCalled()));

  // The "save" action menu item:
  QAction* save_action = new QAction(kSaveActionText, this);
  save_action->setStatusTip(kSaveActionTip);
  save_action->setShortcuts(QKeySequence::Save);
  file_menu->addAction(save_action);
  connect(save_action, SIGNAL(triggered()), this, SLOT(SaveActionCalled()));

  // Initialize the spectra and image layout pointers, shared among the GUI
  // components.
  num_bands_ = std::shared_ptr<int>(new int(kDefaultNumberOfBands));
  spectra_ = std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>>(
      new std::vector<std::shared_ptr<Spectrum>>());
  image_layout_ = std::shared_ptr<ImageLayout>(
      new ImageLayout(kDefaultImageLayoutWidth, kDefaultImageLayoutHeight));

  // Set the tabs with the main GUI components:
  QTabWidget* tabs = new QTabWidget();
  tabs->setParent(this);

  class_spectra_view_ = new ClassSpectraView(num_bands_, spectra_);
  tabs->addTab(class_spectra_view_, kClassSpectraViewString);
  tabs->setTabToolTip(
      tabs->indexOf(class_spectra_view_), kClassSpectraViewToolTip);

  image_layout_view_ = new ImageLayoutView(spectra_, image_layout_);
  tabs->addTab(image_layout_view_, kImageLayoutViewString);
  tabs->setTabToolTip(
      tabs->indexOf(image_layout_view_), kImageLayoutViewToolTip);

  LayoutBlendView* layout_blend_view =
      new LayoutBlendView(spectra_, image_layout_);
  tabs->addTab(layout_blend_view, kLayoutBlendViewString);
  tabs->setTabToolTip(
      tabs->indexOf(layout_blend_view), kLayoutBlendViewToolTip);

  ExportView* export_view = new ExportView(num_bands_, spectra_, image_layout_);
  tabs->addTab(export_view, kExportViewString);
  tabs->setTabToolTip(tabs->indexOf(export_view), kExportViewToolTip);

  setCentralWidget(tabs);
}

void MainWindow::NewActionCalled() {
  MainWindow* new_window = new MainWindow();
  new_window->show();
}

void MainWindow::OpenActionCalled() {
  const QString file_name = QFileDialog::getOpenFileName(
      this,
      kOpenProjectDialogTitle,       // Dialog save caption.
      util::GetRootCodeDirectory(),  // Default directory.
      kXMLFileFilter);               // File filter
  if (!file_name.isEmpty()) {
    ProjectLoader project_loader(spectra_, image_layout_, num_bands_);
    if (project_loader.LoadProjectFromFile(file_name)) {
      setWindowTitle(file_name);
      if (class_spectra_view_ != nullptr) {
        class_spectra_view_->UpdateGUI();
      }
      if (image_layout_view_ != nullptr) {
        image_layout_view_->UpdateGUI();
      }
    } else {
      QMessageBox::critical(
          this,
          kOpenProjectErrorDialogTitle,
          project_loader.GetErrorMessage());
    }
  }
}

void MainWindow::ResetActionCalled() {
  const QMessageBox::StandardButton answer = QMessageBox::question(
      this,
      kResetWarningDialogTitle,
      kResetWarningDialogMessage,
      QMessageBox::Yes | QMessageBox::No);
  if (answer != QMessageBox::Yes) {
    return;
  }
  spectra_->clear();
  image_layout_->ResetLayout();
  if (class_spectra_view_ != nullptr) {
    class_spectra_view_->UpdateGUI();
  }
  if (image_layout_view_ != nullptr) {
    image_layout_view_->UpdateGUI();
  }
}

void MainWindow::SaveActionCalled() {
  const QString file_name = QFileDialog::getSaveFileName(
      this,
      kSaveProjectDialogTitle,       // Dialog save caption.
      util::GetRootCodeDirectory(),  // Default directory.
      kXMLFileFilter);               // File filter
  if (!file_name.isEmpty()) {
    ProjectLoader project_loader(spectra_, image_layout_, num_bands_);
    if (project_loader.SaveProjectToFile(file_name)) {
      setWindowTitle(file_name);
    } else {
      QMessageBox::critical(
          this,
          kSaveProjectErrorDialogTitle,
          project_loader.GetErrorMessage());
    }
  }
}

}  // namespace hsi_data_generator
