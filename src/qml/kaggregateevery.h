/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KAGGREGATEEVERY_H
#define KAGGREGATEEVERY_H

#include "kaggregate.h"

#include <QObject>
#include <QQmlParserStatus>
#include <qqml.h>

class KAggregateEveryAttached;

/**
 * @class KAggregateEvery
 *
 * @brief An aggregate type which reports true value only iff all of the connected objects in its group report true.
 *
 * Following JavaScript Array convention, this class implements Array.prototype.some() kind of functionality
 * for objects that are connected to it via an attached property.
 *
 * @since 6.2
 */
class KAggregateEvery : public QObject, public QQmlParserStatus, public KAggregate<KAggregateEveryAttached, bool>
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(KAggregateEveryAttached)
    QML_ADDED_IN_MINOR_VERSION(2)

    Q_PROPERTY(bool value READ value NOTIFY valueChanged)

public:
    explicit KAggregateEvery(QObject *parent = nullptr);
    ~KAggregateEvery() override;

    void classBegin() override;
    void componentComplete() override;

    using Attached = KAggregateEveryAttached;
    static Attached *qmlAttachedProperties(QObject *parent);

Q_SIGNALS:
    void valueChanged();

private Q_SLOTS:
    void detach(QObject *);

private:
    void attach(KAggregateEveryAttached *);
    void detach(KAggregateEveryAttached *);
    void valueChange(bool newValue);
    void refresh();

    friend class KAggregateEveryAttached;
    friend class KAggregateAttached<KAggregateEvery, bool>;
};

class KAggregateEveryAttached : public QObject, public KAggregateAttached<KAggregateEvery, bool>
{
    Q_OBJECT
    Q_PROPERTY(KAggregateEvery *group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit KAggregateEveryAttached(QObject *parent);
    ~KAggregateEveryAttached() override;

Q_SIGNALS:
    void groupChanged();
    void valueChanged();

protected:
    void emitValueChanged() override;
    void emitGroupChanged() override;
};

QML_DECLARE_TYPEINFO(KAggregateEvery, QML_HAS_ATTACHED_PROPERTIES)

#endif
