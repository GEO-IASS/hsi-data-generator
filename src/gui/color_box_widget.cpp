#include "gui/color_box_widget.h"

#include <QColor>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPalette>
#include <QtGlobal>

namespace hsi_data_generator {
namespace {

constexpr int kNumColorValues = 255;

}  // namespace

ColorBoxWidget::ColorBoxWidget() {
  // Generate random RGB color:
  const int rand_red = qrand() % kNumColorValues;
  const int rand_green = qrand() % kNumColorValues;
  const int rand_blue = qrand() % kNumColorValues;
  SetColor(QColor(rand_red, rand_green, rand_blue));
}

ColorBoxWidget::ColorBoxWidget(const QColor& color) {
  SetColor(color);
}

void ColorBoxWidget::mousePressEvent(QMouseEvent* event) {
  const QColor selected_color = QColorDialog::getColor(color_, this);
  if (selected_color.isValid()) {
    SetColor(selected_color);
  }
}

void ColorBoxWidget::SetColor(const QColor& color) {
  QPalette palette;
  palette.setColor(QPalette::Background, color);
  setAutoFillBackground(true);
  setPalette(palette);
  color_ = color;
}

}  // namespace hsi_data_generator