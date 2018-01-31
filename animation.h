#ifndef ANIMATION_H
#define ANIMATION_H

#include <QGraphicsItem>
#include <QObject>

class AnimationItem : public QGraphicsItem
{
public:
    AnimationItem();
    AnimationItem(int type);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap pix;
};

#endif // ANIMATION_H
