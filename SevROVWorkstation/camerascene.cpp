#include "camerascene.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QToolTip>

#define _USE_MATH_DEFINES
#include <cmath>

CameraScene::CameraScene(QImage img, QObject *parent)
    : QGraphicsScene{parent}
{
    if (!img.isNull())
    {
        screenshot = img.copy();

        // Add Image
        QPixmap img = QPixmap::fromImage(screenshot);
        QGraphicsPixmapItem* item =
            new QGraphicsPixmapItem(img);

        this->addItem(item);                
    }

    lineItem = nullptr;
    circleCurrent = nullptr;
    circleStart = nullptr;
    circleEnd = nullptr;
    textItem = nullptr;    

    setMode(Mode::Undefined);
}

CameraScene::~CameraScene()
{
    if (lineItem != nullptr)
    {
        if (lineItemAdded)
        {
            this->removeItem(lineItem);
            lineItemAdded = false;
        }
        delete lineItem;
    }

    if (circleCurrent != nullptr)
    {
        if (circleCurrentAdded)
        {
            this->removeItem(circleCurrent);
            circleCurrentAdded = false;
        }
        delete circleCurrent;
    }

    if (circleStart != nullptr)
    {
        if (circleStartAdded)
        {
            this->removeItem(circleStart);
            circleStartAdded = false;
        }
        delete circleStart;
    }

    if (circleEnd != nullptr)
    {
        if (circleEndAdded)
        {
            this->removeItem(circleEnd);
            circleEndAdded = false;
        }
        delete circleEnd;
    }

    if (textItem != nullptr)
    {
        if (textItemAdded)
        {
            this->removeItem(textItem);
            textItemAdded = false;
        }
        delete textItem;
    }
}

void CameraScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CameraScene::Mode currentMode = getMode();

    if (event->buttons() & Qt::LeftButton)
    {
        // Начало отрисовки (первое нажатие ЛКМ)
        if ((currentMode == Mode::Undefined) ||
            (currentMode == Mode::RightButton))
        {
            // Сброс отрисовки, если действие не над точкой
            if (!circleFound)
                return;

            startPoint = event->scenePos();
            endPoint = event->scenePos();

            if (circleStart != nullptr)
            {
                if (circleStartAdded)
                {
                    this->removeItem(circleStart);
                    circleStartAdded = false;
                }
            }

            if (circleEnd != nullptr)
            {
                if (circleEndAdded)
                {
                    this->removeItem(circleEnd);
                    circleEndAdded = false;
                }
            }

            if (textItem != nullptr)
            {
                if (textItemAdded)
                {
                    this->removeItem(textItem);
                    textItemAdded = false;
                }
            }

            if (lineItem == nullptr)
            {                
                lineItem = new QGraphicsLineItem();
                lineItem->setPen(QPen(Qt::white, 2, Qt::SolidLine));
            }

            if (lineItem != nullptr)
            {
                lineItem->setLine(0, 0,
                              endPoint.x() - startPoint.x(),
                              endPoint.y() - startPoint.y());
                lineItem->setPos(startPoint);

                if (!lineItemAdded)
                {
                    this->addItem(lineItem);
                    lineItemAdded = true;
                }
            }

            createCircleStart(circleCurrent->rect()); // Создаем первую точку
            circleStartRealX = circleCurrentRealX;
            circleStartRealY = circleCurrentRealY;
            circleStartRealZ = circleCurrentRealZ;
            setMode(Mode::LeftButton); // Устанавливаем режим
        }

        // Окончание отрисовки (второе нажатие ЛКМ)
        if (currentMode == Mode::LeftButton)
        {
            // Сброс отрисовки, если действие не над точкой
            if (!circleFound)
                return;

            endPoint = event->scenePos();
            createCircleEnd(circleCurrent->rect()); // Создаем вторую точку
            circleEndRealX = circleCurrentRealX;
            circleEndRealY = circleCurrentRealY;
            circleEndRealZ = circleCurrentRealZ;
            ///////////////////////////////////////////////////////////////////
            // Добавляем текстовую метку

            double distance = qSqrt(qPow(circleStartRealX - circleEndRealX, 2) +
                                    qPow(circleStartRealY - circleEndRealY, 2) +
                                    qPow(circleStartRealZ - circleEndRealZ, 2));

            ///////////////////////////////////////////////////////////////////
            // Расчет угла поворота текстовой метки
            ///////////////////////////////////////////////////////////////////
            double a = qFabs(endPoint.y() - startPoint.y());
            double b = qFabs(endPoint.x() - startPoint.x());
            double c = qSqrt(a*a + b*b);
            double alpha = 0;

            if (startPoint.x() > endPoint.x())
            {
                if (startPoint.y() < endPoint.y())
                {
                    alpha = qRadiansToDegrees(qAcos(a / c)) - 90;
                }
                if (startPoint.y() > endPoint.y())
                {
                    alpha = qRadiansToDegrees(qAsin(a / c));
                }
            }
            else if (startPoint.x() < endPoint.x())
            {
                if (startPoint.y() < endPoint.y())
                {
                    alpha = qRadiansToDegrees(qAsin(a / c));
                }
                if (startPoint.y() > endPoint.y())
                {
                    alpha = qRadiansToDegrees(qAcos(a / c)) + 270;
                }
            }
            else
                alpha = 0;

            qreal X = (startPoint.x() + endPoint.x()) / 2;
            qreal Y = (startPoint.y() + endPoint.y()) / 2;

            // X, Y - центр отрезка
            createTextItem(X, Y, distance, alpha);
            ///////////////////////////////////////////////////////////////////

            setMode(Mode::Undefined); // Сброс режима
        }

    }
    else if (event->buttons() & Qt::RightButton)
    {
        removeRule(); // Удаляем текущую линейку (точки, линия, метка)
        setMode(Mode::RightButton);
    }

    QGraphicsScene::mousePressEvent(event);
}

void CameraScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    CameraScene::Mode currentMode = getMode();
    circleFound = false;

    // Получаем конечную текущую точку
    endPoint = event->scenePos();

    // Рисуем линию (если отрисовка линейки начата)
    if ((lineItem != nullptr) && (currentMode == Mode::LeftButton))
    {
        lineItem->setLine(0, 0,
                          endPoint.x() - startPoint.x(),
                          endPoint.y() - startPoint.y());
    }

    // Проверяем, попали ли в точку из 3D облака
    for (size_t i = 0; i < clusterPoints.cluster.size(); i++)
    {
        if ((endPoint.x() > clusterPoints.vu.at(i).at(1) - CIRCLE_D / 2) &&
            (endPoint.x() < clusterPoints.vu.at(i).at(1) + CIRCLE_D / 2) &&
            (endPoint.y() > clusterPoints.vu.at(i).at(0) - CIRCLE_D / 2) &&
            (endPoint.y() < clusterPoints.vu.at(i).at(0) + CIRCLE_D / 2))
        {
            // Удаляем предыдущую точку, если такая была найдена ранее
            if (circleCurrent != nullptr)
            {
                if (circleCurrentAdded)
                    this->removeItem(circleCurrent);
                circleCurrentAdded = false;
            }


            // Если ранее точка не была обнаружена,
            // создаем объект и задаем характеристики
            if (circleCurrent == nullptr)
            {
                circleCurrent = new QGraphicsEllipseItem();
                circleCurrent->setPen(QPen(Qt::red, 1, Qt::SolidLine));
            }

            // Отрисовка окружности в области точки
            circleCurrent->setRect(clusterPoints.vu.at(i).at(1) - CIRCLE_D / 2,
                                   clusterPoints.vu.at(i).at(0) - CIRCLE_D / 2,
                                   CIRCLE_D, CIRCLE_D);
            this->addItem(circleCurrent);
            circleCurrentAdded = true;

            circleCurrentRealX = clusterPoints.xyz.at(i).at(0);
            circleCurrentRealY = clusterPoints.xyz.at(i).at(1);
            circleCurrentRealZ = clusterPoints.xyz.at(i).at(2);

            circleFound = true;

            double distance = std::sqrt(
                circleCurrentRealX * circleCurrentRealX +
                circleCurrentRealY * circleCurrentRealY +
                circleCurrentRealZ * circleCurrentRealZ);

            QToolTip::showText(
                event->screenPos(),
                "(" + QString::number(circleCurrentRealX, 'f', 1) + ":"
                    + QString::number(circleCurrentRealY, 'f', 1) + ":"
                    + QString::number(circleCurrentRealZ, 'f', 1) + ") D: "
                    + QString::number(distance, 'f', 1),
                nullptr,
                {},
                5000);

            // Посылаем сигнал
            emit updateInfo(circleCurrentRealX,
                            circleCurrentRealY,
                            circleCurrentRealZ,
                            distance);
            break;
        }
        else
        {
            // Посылаем сигнал
            emit updateInfo(0, 0, 0, 0);
            QToolTip::hideText();
        }
    }

    if ((!circleFound) && (circleCurrent != nullptr))
    {
        if (circleCurrentAdded)
            this->removeItem(circleCurrent);
        circleCurrentAdded = false;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CameraScene::setMode(Mode mode)
{
    sceneMode = mode;
}

CameraScene::Mode CameraScene::getMode()
{
    return sceneMode;
}

void CameraScene::set3DPoints(t_vuxyzrgb points)
{
    clusterPoints.cluster.clear();
    clusterPoints.rgb.clear();
    clusterPoints.vu.clear();
    clusterPoints.xyz.clear();

    // Clear old circles data
    for (size_t i = 0; i < circleItems.size(); i++)
    {
        // Remove the circle form scene
        this->removeItem(circleItems.at(i));

        // Free memory (circle object in vector)
        delete circleItems.at(i);
    }

    // Clear vector of items
    circleItems.clear();

    // Create new circle items
    for (size_t i = 0; i < points.cluster.size(); i++)
    {
        auto elipse = new QGraphicsEllipseItem();
        elipse->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
        // Add real XY from 3D cloud data structure
        elipse->setRect(points.vu.at(i).at(1) - CIRCLE_D / 2,
                        points.vu.at(i).at(0) - CIRCLE_D / 2,
                        CIRCLE_D, CIRCLE_D);
        circleItems.push_back(elipse);
        this->addItem(elipse);

        clusterPoints.cluster.push_back(points.cluster.at(i));
        clusterPoints.rgb.push_back(points.rgb.at(i));
        clusterPoints.vu.push_back(points.vu.at(i));
        clusterPoints.xyz.push_back(points.xyz.at(i));
    }
}

void CameraScene::createCircleStart(QRectF rect)
{
    if (circleStart == nullptr)
    {
        circleStart = new QGraphicsEllipseItem();
        circleStart->setPen(QPen(Qt::red, 2, Qt::SolidLine));
    }
    circleStart->setRect(rect);
    if (!circleStartAdded)
    {
        this->addItem(circleStart);
        circleStartAdded = true;
    }
}
void CameraScene::createCircleEnd(QRectF rect)
{
    if (circleEnd == nullptr)
    {
        circleEnd = new QGraphicsEllipseItem();
        circleEnd->setPen(QPen(Qt::red, 2, Qt::SolidLine));
    }
    circleEnd->setRect(rect);
    if (!circleEndAdded)
    {
        this->addItem(circleEnd);
        circleEndAdded = true;
    }
}
void CameraScene::createTextItem(qreal X, qreal Y, double distance, double angle)
{
    if (textItem == nullptr)
    {
        textItem = new QGraphicsTextItem();
        textItem->setDefaultTextColor(Qt::blue);
    }

    // Задаем шрифт для метки
    QFont font;
    font.setPixelSize(18);
    font.setBold(false);

    textItem->setFont(font);
    //auto W = textItem->boundingRect().width();
    //auto H = textItem->boundingRect().height();
    //textItem->setPos(X + (W/2 * qSin(90 - angle)), Y - (W/2 * qCos(90 - angle)));

    textItem->setPos(X, Y);
    // textItem->setPlainText(QString::number(distance, 'f', 2));
    // Отрисовка метки на белом фоне с 50% прозрачностью
    textItem->setHtml(
        QString("<div style='background:rgba(255, 255, 255, 50%);'>&nbsp;") +
        QString::number(distance, 'f', 2) + QString("&nbsp;</div>"));
    textItem->setRotation(angle);

    if (!textItemAdded)
    {
        this->addItem(textItem);
        textItemAdded = true;
    }
}
void CameraScene::removeRule()
{
    if (circleStart != nullptr)
    {
        if (circleStartAdded)
        {
            this->removeItem(circleStart);
            circleStartAdded = false;
        }
    }

    if (circleEnd != nullptr)
    {
        if (circleEndAdded)
        {
            this->removeItem(circleEnd);
            circleEndAdded = false;
        }
    }

    if (lineItem != nullptr)
    {
        if (lineItemAdded)
        {
            this->removeItem(lineItem);
            lineItemAdded = false;
        }
    }

    if (textItem != nullptr)
    {
        if (textItemAdded)
        {
            this->removeItem(textItem);
            textItemAdded = false;
        }
    }
}
