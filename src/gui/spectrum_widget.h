// This widget acts as a display canvas for the spectrum itself. It provides
// editing and visualization of a single spectrum.

#ifndef SRC_GUI_SPECTRUM_WIDGET_H_
#define SRC_GUI_SPECTRUM_WIDGET_H_

#include <QPaintEvent>
#include <QWidget>

namespace hsi_data_generator {

class SpectrumWidget : public QWidget {
 public:
  SpectrumWidget();

 protected:
  // Uses a QPainter to draw the spectrum, or the current edit visualization,
  // inside the widget.
  void paintEvent(QPaintEvent* event) override;
};

}  // namespace hsi_data_generator

#endif  // SRC_GUI_SPECTRUM_WIDGET_H_
