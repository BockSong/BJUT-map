#include "animation.h"
#include "QGraphicsScene"
#include <QtGui>
#include <QDebug>
#include <math.h>

static const double PI = 3.1415926;

AnimationItem::AnimationItem()
{
    pix.load("images/star.png");
    this->setPos(-430,485);
}

AnimationItem::AnimationItem(int type)
{
    pix.load("images/core.png");
    this->setPos(-430,485);
}

QRectF AnimationItem::boundingRect() const
{
    return QRectF(-pix.width()/2,-pix.height()/2,pix.width(),pix.height());
}

void AnimationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(boundingRect().topLeft(),pix);
}
