#include "DrawGraphWidget.h"
#include <QPainter>

DrawGraphWidget::DrawGraphWidget(QWidget *parent) : QWidget(parent)
{
}

void DrawGraphWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Your graph drawing code here

    QWidget::paintEvent(event);
}
