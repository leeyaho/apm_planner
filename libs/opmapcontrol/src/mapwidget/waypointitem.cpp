/**
******************************************************************************
*
* @file       waypointitem.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      A graphicsItem representing a WayPoint
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
*
*****************************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "waypointitem.h"
#include <QGraphicsSceneMouseEvent>
namespace mapcontrol
{
    WayPointItem::WayPointItem(const internals::PointLatLng &coord,double const& altitude, MapGraphicItem *map) :
        map(map),
        autoreachedEnabled(true),
        text(NULL),
        textBG(NULL),
        numberI(NULL),
        numberIBG(NULL),
        coord(coord),
        reached(false),
        description(""),
        shownumber(true),
        altitude(altitude), // sets a 10m default just in case
        heading(0),
        number(0)
    {
        picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        number=WayPointItem::snumber++;
        setFlag(QGraphicsItem::ItemIsMovable,true);
        setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        setFlag(QGraphicsItem::ItemIsSelectable,true);
        SetShowNumber(shownumber);
        RefreshToolTip();
        RefreshPos();
        setAcceptedMouseButtons(Qt::LeftButton);

        text   = new QGraphicsSimpleTextItem(this);
        text->setPen(QPen(Qt::red));
        text->setPos(10,-picture.height());
        text->setZValue(3);
        text->setVisible(false);

        textBG = new QGraphicsRectItem(this);
        textBG->setBrush(QColor(255, 255, 255, 128));
        textBG->setOpacity(0.5);
        textBG->setPos(10,-picture.height());
        textBG->setVisible(false);
    }

    WayPointItem::WayPointItem(const internals::PointLatLng &coord,double const& altitude, const QString &description, MapGraphicItem *map) :
        map(map),
        autoreachedEnabled(true),
        text(NULL),
        textBG(NULL),
        numberI(NULL),
        numberIBG(NULL),
        coord(coord),
        reached(false),
        description(description),
        shownumber(true),
        altitude(altitude), // sets a 10m default just in case
        heading(0),
        number(0)
    {
        picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        number=WayPointItem::snumber++;
        setFlag(QGraphicsItem::ItemIsMovable,true);
        setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        setFlag(QGraphicsItem::ItemIsSelectable,true);
        SetShowNumber(shownumber);
        RefreshToolTip();
        RefreshPos();
        setAcceptedMouseButtons(Qt::AllButtons);

        // TODO have fun with colors....
        text = new QGraphicsSimpleTextItem(this);
//        text->setPen(QPen(Qt::red));
        text->setBrush(QBrush(Qt::red));
        text->setPos(10,-picture.height());
        text->setZValue(3);
        text->setVisible(false);

        textBG = new QGraphicsRectItem(this);
        textBG->setBrush(QColor(255, 255, 255, 200));
        textBG->setOpacity(0.8);
        textBG->setPos(10,-picture.height());
        textBG->setVisible(false);
    }

    WayPointItem::~WayPointItem()
    {
        WayPointItem::snumber--;
        delete text;
        delete textBG;
    }

    QRectF WayPointItem::boundingRect() const
    {
        return QRectF(-picture.width()/2,-picture.height(),picture.width(),picture.height());
    }

    void WayPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->drawPixmap(-picture.width()/2,-picture.height(),picture);
        if(this->isSelected())
        {
            painter->drawRect(QRectF(-picture.width()/2,-picture.height(),picture.width()-1,picture.height()-1));
        }
    }

    void WayPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton)
        {
            text->setVisible(true);
            textBG->setVisible(true);
            RefreshToolTip();
            QGraphicsItem::mousePressEvent(event);
        }
        else if(event->button() == Qt::RightButton)
        {
            // Deselect if the right button was pressed too
            text->setVisible(false);
            textBG->setVisible(false);
            RefreshToolTip();
            emit WPValuesChanged(this);
            QGraphicsItem::mouseReleaseEvent(event);
        }
    }

    void WayPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton)
        {
            text->setVisible(false);
            textBG->setVisible(false);
            RefreshToolTip();
            emit WPValuesChanged(this);
            QGraphicsItem::mouseReleaseEvent(event);
        }
    }

    void WayPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        coord = map->FromLocalToLatLng(this->pos().x(),this->pos().y());
        QString coord_str = " " + QString::number(coord.Lat(), 'f', 6) + "   " + QString::number(coord.Lng(), 'f', 6);
        text->setText(coord_str);
        textBG->setRect(text->boundingRect());
        emit WPValuesChanged(this);
        QGraphicsItem::mouseMoveEvent(event);
    }

    void WayPointItem::SetAltitude(const double &value)
    {
        altitude=value;
        RefreshToolTip();

        emit WPValuesChanged(this);
        this->update();
    }
    void WayPointItem::SetHeading(const float &value)
    {
        heading=value;
        RefreshToolTip();

        emit WPValuesChanged(this);
        this->update();
    }
    void WayPointItem::SetCoord(const internals::PointLatLng &value)
    {
        coord=value;
        emit WPValuesChanged(this);
        RefreshPos();
        RefreshToolTip();
        this->update();
    }
    void WayPointItem::SetDescription(const QString &value)
    {
        description=value;
        RefreshToolTip();
        emit WPValuesChanged(this);
        this->update();
    }
    void WayPointItem::SetNumber(const int &value)
    {
        emit WPNumberChanged(number,value,this);
        number=value;
        RefreshToolTip();
        numberI->setText(QString::number(number));
        numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
        this->update();
    }
    void WayPointItem::SetReached(const bool &value)
    {
        if (autoreachedEnabled)
        {
            reached=value;
            emit WPValuesChanged(this);
            if(value)
                picture.load(QString::fromUtf8(":/markers/images/bigMarkerGreen.png"));
            else
                picture.load(QString::fromUtf8(":/markers/images/marker.png"));
            this->update();
        }
    }
    void WayPointItem::SetShowNumber(const bool &value)
    {
        shownumber=value;
        if((numberI==NULL) && value)
        {
            Q_ASSERT(numberI == NULL);
            Q_ASSERT(numberIBG == NULL);
            numberI=new QGraphicsSimpleTextItem(this);
            numberIBG=new QGraphicsRectItem(this);
            numberIBG->setBrush(Qt::black);
            numberIBG->setOpacity(0.5);
            numberI->setZValue(3);
            numberI->setBrush(Qt::white);
            numberI->setPos(18,-picture.height()/2-2);
            numberIBG->setPos(18,-picture.height()/2-2);
            numberI->setText(QString::number(number));
            numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
        }
        else if (!value && numberI)
        {
            delete numberI;
            numberI = NULL;
            delete numberIBG;
            numberIBG = NULL;
        }
        this->update();

    }
    void WayPointItem::WPDeleted(const int &onumber)
    {
        if(number>onumber) --number;
        numberI->setText(QString::number(number));
        numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
        RefreshToolTip();
        this->update();
    }
    void WayPointItem::WPInserted(const int &onumber, WayPointItem *waypoint)
    {
        if(waypoint!=this)
        {
            if(onumber<=number) ++number;
            numberI->setText(QString::number(number));
            RefreshToolTip();
            this->update();
        }
    }
    void WayPointItem::WPRenumbered(const int &oldnumber, const int &newnumber, WayPointItem *waypoint)
    {
        if (waypoint!=this)
        {
            if(((oldnumber>number) && (newnumber<=number)))
            {
                ++number;
                numberI->setText(QString::number(number));
                numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
                RefreshToolTip();
            }
            else if (((oldnumber<number) && (newnumber>number)))
            {
                --number;
                numberI->setText(QString::number(number));
                numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
                RefreshToolTip();
            }
            else if (newnumber==number)
            {
                ++number;
                numberI->setText(QString::number(number));
                RefreshToolTip();
            }
            this->update();
        }
    }
    int WayPointItem::type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }


    void WayPointItem::RefreshPos()
    {
        core::Point point=map->FromLatLngToLocal(coord);
        this->setPos(point.X(),point.Y());
    }
    void WayPointItem::RefreshToolTip()
    {
        QString coord_str = QString::number(coord.Lat(), 'f', 6) + "   " + QString::number(coord.Lng(), 'f', 6);
        setToolTip(QString("WayPoint Number: %1\nDescription: %2\nCoordinate: %4\nAltitude: %5 m\nHeading: %6 deg").arg(QString::number(WayPointItem::number)).arg(description).arg(coord_str).arg(QString::number(altitude)).arg(QString::number(heading)));
    }

    int WayPointItem::snumber=0;
}
