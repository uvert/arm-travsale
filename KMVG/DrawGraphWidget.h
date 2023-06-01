#ifndef DRAWGRAPHWIDGET_H
#define DRAWGRAPHWIDGET_H

#include <QWidget>

class DrawGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawGraphWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void drawGraph();

};

#endif // DRAWGRAPHWIDGET_H
