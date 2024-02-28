/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KAGGREGATE_H
#define KAGGREGATE_H

#include <QObject>
#include <QPointer>

#include <memory>

template<typename Attached, typename Data>
class KAggregatePrivate;

/**
 * @class KAggregate
 *
 * @brief A base class for aggregate property groups.
 *
 * KAggregate implements the low-level details of managing a group of objects
 * back-linked by a set of attached objects.
 *
 * @since 6.1
 */
template<typename Attached, typename Data>
class KAggregate
{
public:
    explicit KAggregate(Data value);
    ~KAggregate();

    /**
     * Returns the contained value.
     */
    inline Data value() const;

protected:
    /**
     * Returns whether the value has changed.
     */
    bool setValue(Data data);

    inline QList<QPointer<Attached>> &objects();

    inline bool completed() const;
    inline void setCompleted();

private:
    std::unique_ptr<KAggregatePrivate<Attached, Data>> const d;
};

template<typename Attached, typename Data>
class KAggregatePrivate
{
    KAggregate<Attached, Data> *const q;
    QList<QPointer<Attached>> objects;
    Data value;
    bool completed;

    friend class KAggregate<Attached, Data>;

public:
    explicit KAggregatePrivate(KAggregate<Attached, Data> *qq, Data vv)
        : q(qq)
        , value(vv)
        , completed(false)
    {
    }
};

template<typename Attached, typename Data>
KAggregate<Attached, Data>::KAggregate(Data value)
    : d(new KAggregatePrivate<Attached, Data>(this, value))
{
}

template<typename Attached, typename Data>
KAggregate<Attached, Data>::~KAggregate() = default;

template<typename Attached, typename Data>
inline Data KAggregate<Attached, Data>::value() const
{
    return d->value;
}

template<typename Attached, typename Data>
bool KAggregate<Attached, Data>::setValue(Data value)
{
    const auto changed = d->value != value;
    if (changed) {
        d->value = value;
    }
    return changed;
}

template<typename Attached, typename Data>
inline QList<QPointer<Attached>> &KAggregate<Attached, Data>::objects()
{
    return d->objects;
}

template<typename Attached, typename Data>
inline bool KAggregate<Attached, Data>::completed() const
{
    return d->completed;
}

template<typename Attached, typename Data>
inline void KAggregate<Attached, Data>::setCompleted()
{
    d->completed = true;
}

template<typename Attachee, typename Data>
class KAggregateAttached
{
public:
    explicit KAggregateAttached(Data value);
    virtual ~KAggregateAttached();

    Attachee *group() const;
    void setGroup(Attachee *group);

    Data value() const;
    void setValue(Data value);

protected:
    virtual void emitValueChanged() = 0;
    virtual void emitGroupChanged() = 0;

private:
    inline Attachee::Attached *asAttached()
    {
        return static_cast<typename Attachee::Attached *>(this);
    }

    QPointer<Attachee> m_group;
    Data m_value;
};

template<typename Attachee, typename Data>
KAggregateAttached<Attachee, Data>::KAggregateAttached(Data value)
    : m_value(value)
{
}

template<typename Attachee, typename Data>
KAggregateAttached<Attachee, Data>::~KAggregateAttached()
{
    if (m_group) {
        m_group->detach(asAttached());
        m_group = nullptr;
    }
}

template<typename Attachee, typename Data>
Attachee *KAggregateAttached<Attachee, Data>::group() const
{
    return m_group;
}

template<typename Attachee, typename Data>
void KAggregateAttached<Attachee, Data>::setGroup(Attachee *group)
{
    if (m_group == group) {
        return;
    }

    if (m_group) {
        m_group->detach(asAttached());
    }

    m_group = group;

    if (m_group) {
        m_group->attach(asAttached());
        // TODO: connect to group::destroyed?
    }

    emitGroupChanged();
}

template<typename Attachee, typename Data>
Data KAggregateAttached<Attachee, Data>::value() const
{
    return m_value;
}

template<typename Attachee, typename Data>
void KAggregateAttached<Attachee, Data>::setValue(Data value)
{
    if (m_value == value) {
        return;
    }

    m_value = value;

    if (m_group) {
        m_group->valueChange(value);
    }

    emitValueChanged();
}

#endif
