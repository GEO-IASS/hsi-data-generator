// This widget displays and allows editing of a single class spectrum. This
// spectrum and others is used to generate the synthetic image.

#ifndef SRC_GUI_CLASS_SPECTRUM_ROW_H_
#define SRC_GUI_CLASS_SPECTRUM_ROW_H_

#include <QString>
#include <QWidget>

namespace hsi_data_generator {

class ClassSpectrumRow : public QWidget {
 public:
  explicit ClassSpectrumRow(const QString& class_name);  // TODO: More params.
};

}  // namespace hsi_data_generator

#endif  // SRC_GUI_CLASS_SPECTRUM_ROW_H_