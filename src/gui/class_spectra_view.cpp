#include "gui/class_spectra_view.h"

#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QtDebug>
#include <QVBoxLayout>

#include <algorithm>
#include <memory>
#include <vector>

#include "gui/class_spectrum_row.h"
#include "spectrum/spectrum.h"
#include "util/util.h"

namespace hsi_data_generator {
namespace {

static const QString kQtClassSpectraViewStyle =
    "qt_stylesheets/class_spectra_view.qss";

// The default number of bands that is pre-set until the user changes it.
constexpr int kDefaultNumberOfBands = 100;

static const QString kDefaultSpectrumName = "Background";
static const QColor kDefaultSpectrumColor = Qt::black;

static const QString kNewSpectrumButtonString = "Add Spectrum";

}  // namespace

ClassSpectraView::ClassSpectraView(
    std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>> spectra)
    : num_bands_(kDefaultNumberOfBands),
      next_spectrum_number_(1),
      spectra_(spectra) {

  setStyleSheet(util::GetStylesheetRelativePath(kQtClassSpectraViewStyle));

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
  layout_->setAlignment(new_spectrum_button, Qt::AlignCenter);
  connect(
      new_spectrum_button,
      SIGNAL(released()),
      this,
      SLOT(NewSpectrumButtonPressed()));

  // Add a default spectrum to begin with (typically the background spectrum).
  InsertNewSpectrum(kDefaultSpectrumName);
}

void ClassSpectraView::NumberOfBandsInputChanged() {
  if (number_of_bands_input_ == nullptr) {
    qCritical() << "Number of bands input is not defined. Cannot change.";
    return;
  }
  const QString num_bands_string = number_of_bands_input_->text();
  num_bands_ = num_bands_string.toInt();
  for (int i = 0; i < class_spectrum_rows_.size(); ++i) {
    class_spectrum_rows_[i]->SetNumberOfBands(num_bands_);
  }
}

void ClassSpectraView::NewSpectrumButtonPressed() {
  QString new_spectrum_name =
      "New Spectrum " + QString::number(next_spectrum_number_);
  next_spectrum_number_++;
  InsertNewSpectrum(new_spectrum_name);
}

void ClassSpectraView::RowDeleteButtonPressed(QWidget* caller) {
  qInfo() << "Worked!";
}

void ClassSpectraView::RowCloneButtonPressed(QWidget* caller) {
  const ClassSpectrumRow* spectrum_row =
      dynamic_cast<ClassSpectrumRow*>(caller);
  std::shared_ptr<Spectrum> spectrum_copy = spectrum_row->GetSpectrumCopy();
  AddClassSpectrumRow(spectrum_copy);
}

void ClassSpectraView::InsertNewSpectrum(const QString& name) {
  std::shared_ptr<Spectrum> spectrum(new Spectrum(name, kDefaultSpectrumColor));
  AddClassSpectrumRow(spectrum);
}

void ClassSpectraView::AddClassSpectrumRow(std::shared_ptr<Spectrum> spectrum) {
  spectra_->push_back(spectrum);
  ClassSpectrumRow* row = new ClassSpectrumRow(num_bands_, spectrum, this);
  class_spectrum_rows_.push_back(row);
  // Insert as the second-to-last item, since the last item should always be
  // the new spectrum button.
  int new_row_index = std::max(layout_->count() - 1, 0);
  layout_->insertWidget(new_row_index, row);
}

}  // namespace hsi_data_generator
