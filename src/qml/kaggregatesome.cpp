/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kaggregatesome.h"

#include <QOverload>
#include <QQmlInfo>

#include <algorithm>

KAggregateSome::KAggregateSome(QObject *parent)
    : QObject(parent)
    , KAggregate<KAggregateSomeAttached, bool>(false)
{
}

KAggregateSome::~KAggregateSome()
{
    // Optimized version of detach()
    const auto &members = objects();
    for (const auto &member : members) {
        disconnect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateSome::detach));
    }
    setValue(false);
}

void KAggregateSome::classBegin()
{
}

void KAggregateSome::componentComplete()
{
    setCompleted();
    refresh();
}

KAggregateSomeAttached *KAggregateSome::qmlAttachedProperties(QObject *parent)
{
    Q_ASSERT(parent);
    return new KAggregateSomeAttached(parent);
}

void KAggregateSome::attach(KAggregateSomeAttached *member)
{
    if (!member || objects().contains(member)) {
        return;
    }

    objects().append(member);
    connect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateSome::detach));
    valueChange(member->value());
}

void KAggregateSome::detach(QObject *object)
{
    if (const auto member = qobject_cast<KAggregateSomeAttached *>(object)) {
        detach(member);
    }
}

void KAggregateSome::detach(KAggregateSomeAttached *member)
{
    if (!member || !objects().contains(member)) {
        return;
    }

    objects().removeOne(member);
    disconnect(member, &QObject::destroyed, this, qOverload<QObject *>(&KAggregateSome::detach));
    valueChange(false);
}

void KAggregateSome::valueChange(bool newValue)
{
    if (value() != newValue) {
        refresh();
    }
}

void KAggregateSome::refresh()
{
    if (!completed()) {
        return;
    }

    const auto &members = objects();
    const auto aggregate = std::find_if(members.begin(),
                                        members.end(),
                                        [](const auto &member) {
                                            return member && member->value();
                                        })
        != members.end();

    if (setValue(aggregate)) {
        Q_EMIT valueChanged();
    }
}

KAggregateSomeAttached::KAggregateSomeAttached(QObject *parent)
    : QObject(parent)
    , KAggregateAttached<KAggregateSome, bool>(false)
{
    Q_ASSERT(parent);
}

KAggregateSomeAttached::~KAggregateSomeAttached() = default;

void KAggregateSomeAttached::emitValueChanged()
{
    Q_EMIT valueChanged();
}

void KAggregateSomeAttached::emitGroupChanged()
{
    Q_EMIT groupChanged();
}

#include "moc_kaggregatesome.cpp"
