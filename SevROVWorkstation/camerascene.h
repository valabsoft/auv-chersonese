#ifndef CAMERASCENE_H
#define CAMERASCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "datastructure.h"

class CameraScene : public QGraphicsScene
{
    Q_OBJECT
public:    
    explicit CameraScene(QImage img, QObject *parent = nullptr);
    ~CameraScene();

    static const int CIRCLE_D = 10;
    enum Mode {Undefined, LeftButton, RightButton};

    void setMode(Mode mode);
    Mode getMode();

    void set3DPoints(t_vuxyzrgb points);
    void removeRule();

private:
    QPointF startPoint;
    QPointF endPoint;
    QImage screenshot;

    QGraphicsLineItem* lineItem;
    std::vector<QGraphicsEllipseItem*> circleItems;
    QGraphicsEllipseItem* circleCurrent;
    QGraphicsEllipseItem* circleStart;
    QGraphicsEllipseItem* circleEnd;
    QGraphicsTextItem* textItem;

    // TODO: Использование данного флага - костыль.
    // Подумать как оптимизировать
    bool lineItemAdded = false;
    bool circleCurrentAdded = false;
    bool circleStartAdded = false;
    bool circleEndAdded = false;
    bool textItemAdded = false;

    bool circleFound = false;

    double circleCurrentRealX = 0;
    double circleCurrentRealY = 0;
    double circleCurrentRealZ = 0;

    double circleStartRealX = 0;
    double circleStartRealY = 0;
    double circleStartRealZ = 0;

    double circleEndRealX = 0;
    double circleEndRealY = 0;
    double circleEndRealZ = 0;

    Mode sceneMode;
    t_vuxyzrgb clusterPoints;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void createCircleStart(QRectF rect);
    void createCircleEnd(QRectF rect);
    void createTextItem(qreal X, qreal Y, double distance, double angle);

signals:
    void updateInfo(double X, double Y, double Z, double D);
};

#endif // CAMERASCENE_H
