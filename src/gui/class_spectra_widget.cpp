#include "gui/class_spectra_widget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QtDebug>
#include <QVBoxLayout>

#include <algorithm>

#include "gui/class_spectrum_row.h"

namespace hsi_data_generator {
namespace {

// The default number of bands that is pre-set until the user changes it.
constexpr int kDefaultNumberOfBands = 100;

static const QString kDefaultSpectrumName = "Background";
static const QString kNewSpectrumButtonString = "Add Spectrum";

}  // namespace

ClassSpectraWidget::ClassSpectraWidget()
    : num_bands_(kDefaultNumberOfBands), next_spectrum_number_(1) {

  layout_ = new QVBoxLayout();
  layout_->setAlignment(Qt::AlignTop);
  setLayout(layout_);

  // Add the input field to change the number of spectral bands.
  number_of_bands_input_ =
      new QLineEdit(QString::number(num_bands_));
  layout_->addWidget(number_of_bands_input_);
  connect(
      number_of_bands_input_,
      SIGNAL(returnPressed()),
      this,
      SLOT(NumberOfBandsInputChanged()));

  // Add a button to add a new spectrum. Pushing this button will add a new
  // blank spectrum to the list.
  QPushButton* new_spectrum_button = new QPushButton(kNewSpectrumButtonString);
  layout_->addWidget(new_spectrum_button);
  connect(
      new_spectrum_button,
      SIGNAL(released()),
      this,
      SLOT(NewSpectrumButtonPressed()));

  // Add a default spectrum to begin with (typically the background spectrum).
  AddClassSpectrumRow(kDefaultSpectrumName, num_bands_);
}

void ClassSpectraWidget::NumberOfBandsInputChanged() {
  if (number_of_bands_input_ == nullptr) {
    qCritical() << "Number of bands input is not defined. Cannot change.";
    return;
  }
  const QString num_bands_string = number_of_bands_input_->text();
  num_bands_ = num_bands_string.toInt();
  for (ClassSpectrumRow* row : class_spectrum_rows_) {
    row->SetNumberOfBands(num_bands_);
  }
}

void ClassSpectraWidget::NewSpectrumButtonPressed() {
  QString new_spectrum_name =
      "New Spectrum " + QString::number(next_spectrum_number_);
  next_spectrum_number_++;
  AddClassSpectrumRow(new_spectrum_name, num_bands_);
}

void ClassSpectraWidget::AddClassSpectrumRow(
    const QString& name, const int num_bands) {

  ClassSpectrumRow* row = new ClassSpectrumRow(name, num_bands);
  // Insert as the second-to-last item, since the last item should always be
  // the new spectrum button.
  int new_row_index = std::max(layout_->count() - 1, 0);
  layout_->insertWidget(new_row_index, row);
  class_spectrum_rows_.push_back(row);
}

}  // namespace hsi_data_generator
