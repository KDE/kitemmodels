/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kaggregateevery.h"

#include <QOverload>
#include <QQmlInfo>

#include <algorithm>

KAggregateEvery::KAggregateEvery(QObject *parent)
    : QObject(parent)
    , KAggregate<KAggregateEveryAttached, bool>(true)
{
}

KAggregateEvery::~KAggregateEvery()
{
    // Optimized version of detach()
    const auto &members = objects();
    for (const auto &member : members) {
        disconnect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateEvery::detach));
    }
    setValue(false);
}

void KAggregateEvery::classBegin()
{
}

void KAggregateEvery::componentComplete()
{
    setCompleted();
    refresh();
}

KAggregateEveryAttached *KAggregateEvery::qmlAttachedProperties(QObject *parent)
{
    Q_ASSERT(parent);
    return new KAggregateEveryAttached(parent);
}

void KAggregateEvery::attach(KAggregateEveryAttached *member)
{
    if (!member || objects().contains(member)) {
        return;
    }

    objects().append(member);
    connect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateEvery::detach));
    valueChange(member->value());
}

void KAggregateEvery::detach(QObject *object)
{
    if (const auto member = qobject_cast<KAggregateEveryAttached *>(object)) {
        detach(member);
    }
}

void KAggregateEvery::detach(KAggregateEveryAttached *member)
{
    if (!member || !objects().contains(member)) {
        return;
    }

    objects().removeOne(member);
    disconnect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateEvery::detach));
    valueChange(false);
}

void KAggregateEvery::valueChange(bool newValue)
{
    if (value() != newValue) {
        refresh();
    }
}

void KAggregateEvery::refresh()
{
    if (!completed()) {
        return;
    }

    const auto &members = objects();
    const auto aggregate = std::find_if(members.begin(),
                                        members.end(),
                                        [](const auto &member) {
                                            return member && !member->value();
                                        })
        == members.end();

    if (setValue(aggregate)) {
        Q_EMIT valueChanged();
    }
}

KAggregateEveryAttached::KAggregateEveryAttached(QObject *parent)
    : QObject(parent)
    , KAggregateAttached<KAggregateEvery, bool>(false)
{
    Q_ASSERT(parent);
}

KAggregateEveryAttached::~KAggregateEveryAttached() = default;

void KAggregateEveryAttached::emitValueChanged()
{
    Q_EMIT valueChanged();
}

void KAggregateEveryAttached::emitGroupChanged()
{
    Q_EMIT groupChanged();
}

#include "moc_kaggregateevery.cpp"
