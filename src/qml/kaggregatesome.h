/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KAGGREGATESOME_H
#define KAGGREGATESOME_H

#include "kaggregate.h"

#include <QObject>
#include <QQmlParserStatus>
#include <qqml.h>

class KAggregateSomeAttached;

/**
 * @class KAggregateSome
 *
 * @brief An aggregate type which reports true value only if any of the connected objects in its group report true.
 *
 * Following JavaScript Array convention, this class implements Array.prototype.some() kind of functionality
 * for objects that are connected to it via an attached property.
 *
 * @since 6.2
 */
class KAggregateSome : public QObject, public QQmlParserStatus, public KAggregate<KAggregateSomeAttached, bool>
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(KAggregateSomeAttached)
    QML_ADDED_IN_MINOR_VERSION(2)

    Q_PROPERTY(bool value READ value NOTIFY valueChanged)

public:
    explicit KAggregateSome(QObject *parent = nullptr);
    ~KAggregateSome() override;

    void classBegin() override;
    void componentComplete() override;

    using Attached = KAggregateSomeAttached;
    static Attached *qmlAttachedProperties(QObject *parent);

Q_SIGNALS:
    void valueChanged();

private Q_SLOTS:
    void detach(QObject *);

private:
    void attach(KAggregateSomeAttached *);
    void detach(KAggregateSomeAttached *);
    void valueChange(bool newValue);
    void refresh();

    friend class KAggregateSomeAttached;
    friend class KAggregateAttached<KAggregateSome, bool>;
};

class KAggregateSomeAttached : public QObject, public KAggregateAttached<KAggregateSome, bool>
{
    Q_OBJECT
    Q_PROPERTY(KAggregateSome *group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit KAggregateSomeAttached(QObject *parent);
    ~KAggregateSomeAttached() override;

Q_SIGNALS:
    void groupChanged();
    void valueChanged();

protected:
    void emitValueChanged() override;
    void emitGroupChanged() override;
};

QML_DECLARE_TYPEINFO(KAggregateSome, QML_HAS_ATTACHED_PROPERTIES)

#endif
