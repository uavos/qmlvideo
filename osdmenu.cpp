#include "osdmenu.h"

#include <QtQml/qqml.h>
#include <QPainter>
#include <iostream>

void OsdMenu::registerQmlType()
{
    qmlRegisterType<OsdMenu>("OsdMenu", 10, 0, "OsdMenu");
}

void OsdMenu::drawOsdMenu(QImage *image)
{
    int y = 20;
    int x = 5;
    QPainter painter(image);
    painter.setPen(QColor(Qt::red));

    QString pattern = "%1: %2";
    for(QObject *item: m_items)
    {
        OsdItem *osdItem = static_cast<OsdItem*>(item);
        if(osdItem)
        {
            QString name = osdItem->name();
            QString value = osdItem->value().toString();
            painter.drawText(x, y, pattern.arg(name).arg(value));
            y += 30;
        }
        else
            std::cerr << "Not OsdItem object detected" << std::endl;
    }

    //aim
    painter.drawLine(QPointF(image->width() / 2 - 50, image->height() / 2),
                     QPointF(image->width() / 2 + 50, image->height() / 2));
    painter.drawLine(QPointF(image->width() / 2, image->height() / 2 - 50),
                     QPointF(image->width() / 2, image->height() / 2 + 50));
}

OsdMenu::OsdMenu()
{

}

QQmlListProperty<QObject> OsdMenu::items()
{
    return QQmlListProperty<QObject>(this, 0, &appendElements, &elementsCount,
                                     &elementsAt, &elementsClear);
}

void OsdMenu::appendElements(QQmlListProperty<QObject> *list, QObject *e)
{
    OsdMenu *osdMenu = static_cast<OsdMenu*>(list->object);
    osdMenu->m_items.append(e);
    osdMenu->emitItemsChanged();
}

int OsdMenu::elementsCount(QQmlListProperty<QObject> *list)
{
    OsdMenu *osdMenu = static_cast<OsdMenu*>(list->object);
    return osdMenu->m_items.size();
}

QObject *OsdMenu::elementsAt(QQmlListProperty<QObject> *list, int i)
{
    OsdMenu *osdMenu = static_cast<OsdMenu*>(list->object);
    return osdMenu->m_items.value(i);
}

void OsdMenu::elementsClear(QQmlListProperty<QObject> *list)
{
    OsdMenu *osdMenu = static_cast<OsdMenu*>(list->object);
    osdMenu->m_items.clear();
    osdMenu->emitItemsChanged();
}

void OsdMenu::emitItemsChanged()
{
    emit itemsChanged();
}

void OsdItem::registerQmlType()
{
    qmlRegisterType<OsdItem>("OsdItem", 10, 0, "OsdItem");
}

OsdItem::OsdItem():
    OsdItem("Unknown", -1, 2)
{
}

OsdItem::OsdItem(QString name, QVariant value, int precision):
    m_name(name),
    m_value(value),
    m_precision(precision)
{
}

QString OsdItem::name() const
{
    return m_name;
}

void OsdItem::setName(const QString &name)
{
    m_name = name;
    emit nameChanged();
}

QVariant OsdItem::value() const
{
    if(m_value.type() == QVariant::String)
        return m_value;
    else
        return QString::number(m_value.toDouble(), 'f', m_precision);
}

void OsdItem::setValue(QVariant value)
{
    m_value = value;
    emit valueChanged();
}

int OsdItem::precision() const
{
    return m_precision;
}

void OsdItem::setPrecision(int precision)
{
    m_precision = precision;
}
