#ifndef OSDMENU_H
#define OSDMENU_H

#include <QObject>
#include <QVariant>
#include <QtQml/QQmlListProperty>

class OsdMenu: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> items READ items NOTIFY itemsChanged)
public:
    OsdMenu();

    static void registerQmlType();

    void drawOsdMenu(QImage *image);

    QQmlListProperty<QObject> items();
    void emitItemsChanged();
    static void appendElements(QQmlListProperty<QObject> *list, QObject *e);
    static int elementsCount(QQmlListProperty<QObject> *list);
    static QObject* elementsAt(QQmlListProperty<QObject> *list, int i);
    static void elementsClear(QQmlListProperty<QObject> *list);

private:
    QList<QObject*> m_items;

signals:
    void itemsChanged();
};

class OsdItem: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int precision READ precision WRITE setPrecision NOTIFY precisionChanged)

public:
    OsdItem();

    static void registerQmlType();

    OsdItem(QString name, QVariant value, int precision);
    QString name() const;
    void setName(const QString &name);
    QVariant value() const;
    void setValue(QVariant value);
    int precision() const;
    void setPrecision(int precision);

private:
    QString m_name;
    QVariant m_value;
    int m_precision;

signals:
    void valueChanged();
    void nameChanged();
    void precisionChanged();
};

#endif // OSDMENU_H
