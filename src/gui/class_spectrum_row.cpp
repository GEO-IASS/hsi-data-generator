#include "gui/class_spectrum_row.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QString>
#include <QtDebug>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

#include "gui/class_spectra_view.h"
#include "gui/color_box_widget.h"
#include "gui/spectrum_widget.h"
#include "hsi/spectrum.h"
#include "util/util.h"

namespace hsi_data_generator {
namespace {

// Stylesheet file path and object reference names.
static const QString kQtSpectrumRowViewStyle =
    "qt_stylesheets/class_spectrum_row.qss";
static const QString kQtClassColorBoxName = "class_color_box";

// The text displayed on the edit button during its two modes as it gets
// toggled between them.
static const QString kEditButtonEditText = "Edit";
static const QString kEditButtonDoneText = "Done";
static const QString kEditButtonToolTip(
    "Toggle edit mode. In edit mode, you can click on the spectrum to "
    "add, remove, or modify the Gaussian peaks that define it.");
static const QString kClearButtonText = "Clear";
static const QString kClearButtonToolTip(
    "Removes all the peaks to reset this spectrum to be like new. "
    "Press again to delete the spectrum entirely.");
static const QString kCloneButtonText = "Clone";
static const QString kCloneButtonToolTip(
    "Adds a new spectrum that is an exact copy of this spectrum. "
    "You can edit the new spectrum to make changes later.");

// Other tooltips:
static const QString kClassNameInputToolTip(
    "Rename this spectrum. "
    "This name does not impact the final exported HSI file.");
static const QString kColorBoxToolTip(
    "Change the color of this spectrum. "
    "This color is for visualization purposes only.");

// Prompt dialog strings:
static const QString kDeleteSpectrumName = "Delete Spectrum";
static const QString kDeleteSpectrumQuestionString =
    "Delete spectrum \"" + util::kTextSubPlaceholder + "\"?";

}  // namespace

ClassSpectrumRow::ClassSpectrumRow(
    const int num_bands,
    std::shared_ptr<Spectrum> spectrum,
    QWidget* parent_view)
    : parent_view_(parent_view), spectrum_(spectrum) {

  setStyleSheet(util::GetStylesheetRelativePath(kQtSpectrumRowViewStyle));

  QHBoxLayout* layout = new QHBoxLayout();
  layout->setAlignment(Qt::AlignLeft);
  setLayout(layout);

  QLineEdit* class_name_field = new QLineEdit(spectrum_->GetName());
  class_name_field->setToolTip(kClassNameInputToolTip);
  layout->addWidget(class_name_field);
  connect(
      class_name_field,
      SIGNAL(textChanged(const QString&)),
      this,
      SLOT(ClassNameFieldChanged(const QString&)));

  ColorBoxWidget* class_color_box = new ColorBoxWidget(spectrum_);
  class_color_box->setToolTip(kColorBoxToolTip);
  class_color_box->setObjectName(kQtClassColorBoxName);  // Name for stylesheet.
  layout->addWidget(class_color_box);

  spectrum_widget_ = new SpectrumWidget(num_bands, spectrum_);
  layout->addWidget(spectrum_widget_);

  // Add the buttons. Stack these vertically at the end of the row.
  QVBoxLayout* button_layout = new QVBoxLayout();

  spectrum_edit_button_ = new QPushButton(kEditButtonEditText);
  spectrum_edit_button_->setToolTip(kEditButtonToolTip);
  button_layout->addWidget(spectrum_edit_button_);
  connect(
      spectrum_edit_button_,
      SIGNAL(released()),
      this,
      SLOT(EditButtonPressed()));

  QPushButton* clear_button = new QPushButton(kClearButtonText);
  clear_button->setToolTip(kClearButtonToolTip);
  button_layout->addWidget(clear_button);
  connect(clear_button, SIGNAL(released()), this, SLOT(ClearButtonPressed()));

  QPushButton* clone_button = new QPushButton(kCloneButtonText);
  clone_button->setToolTip(kCloneButtonToolTip);
  button_layout->addWidget(clone_button);

  // Map the clone button to the parent ClassSpectraView's slot method. The
  // signal mapper allows this ClassSpectrumRow to be passed down as a
  // parameter to the slot method.
  QSignalMapper* signal_mapper = new QSignalMapper(parent_view);
  connect(
      clone_button,
      SIGNAL(released()),
      signal_mapper,
      SLOT(map()));
  signal_mapper->setMapping(clone_button, this);
  connect(
      signal_mapper,
      SIGNAL(mapped(QWidget*)),
      parent_view,
      SLOT(RowCloneButtonPressed(QWidget*)));

  layout->addLayout(button_layout);
}

void ClassSpectrumRow::SetNumberOfBands(const int num_bands) {
  if (spectrum_widget_ == nullptr) {
    qCritical() << "Spectrum widget not defined. Cannot set number of bands.";
    return;
  }
  spectrum_widget_->SetNumberOfBands(num_bands);
}

std::shared_ptr<Spectrum> ClassSpectrumRow::GetSpectrumCopy() const {
  std::shared_ptr<Spectrum> spectrum_copy(new Spectrum(*spectrum_));
  spectrum_copy->SetName("Copy of " + spectrum_->GetName());
  return spectrum_copy;
}

void ClassSpectrumRow::ClassNameFieldChanged(const QString& class_name) {
  if (!class_name.isEmpty()) {
    spectrum_->SetName(class_name);
  }
}

void ClassSpectrumRow::EditButtonPressed() {
  // TODO: Better error checks?
  if (spectrum_widget_ == nullptr) {
    qCritical() << "Spectrum widget is not defined. Cannot edit.";
    return;
  }
  if (spectrum_edit_button_ == nullptr) {
    qCritical() << "Edit button is not defined. Cannot edit.";
    return;
  }
  if (spectrum_widget_->GetDisplayMode() == SPECTRUM_RENDER_MODE) {
    spectrum_widget_->SetDisplayMode(SPECTRUM_EDIT_MODE);
    spectrum_edit_button_->setText(kEditButtonDoneText);
  } else {
    spectrum_widget_->SetDisplayMode(SPECTRUM_RENDER_MODE);
    spectrum_edit_button_->setText(kEditButtonEditText);
  }
}

void ClassSpectrumRow::ClearButtonPressed() {
  if (spectrum_widget_ == nullptr) {
    qCritical() << "Spectrum widget is not defined. Cannot clear it.";
    return;
  }
  // If the clear button is pressed and the spectrum is currently clear, prompt
  // the user to delete the row. Otherwise, just clear the spectrum.
  if (spectrum_->IsEmpty()) {
    const QString prompt_question = util::ReplaceTextSubPlaceholder(
        kDeleteSpectrumQuestionString, spectrum_->GetName());
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        kDeleteSpectrumName,
        prompt_question,
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      ClassSpectraView* parent_view =
          dynamic_cast<ClassSpectraView*>(parent_view_);
      parent_view->DeleteClassSpectrumRow(this);
    }
  } else {
    spectrum_widget_->Clear();
  }
}

}  // namespace hsi_data_generator
