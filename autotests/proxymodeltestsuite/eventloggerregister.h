/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EVENTLOGGERREGISTER_H
#define EVENTLOGGERREGISTER_H

#include <QVector>

#include <memory>

#include "proxymodeltestsuite_export.h"

class ModelEventLogger;

class PROXYMODELTESTSUITE_EXPORT EventLoggerRegister
{
public:
    enum Behaviour {
        InstallMsgHandler,
        NoInstallMsgHandler
    };
    static EventLoggerRegister *instance(Behaviour behaviour = InstallMsgHandler);

    void registerLogger(ModelEventLogger *logger);
    void unregisterLogger(ModelEventLogger *logger);

    void writeLogs();

private:
    EventLoggerRegister(Behaviour behaviour);
    ~EventLoggerRegister();
    QVector<ModelEventLogger *> m_loggers;

    static EventLoggerRegister *s_instance;
    friend class std::auto_ptr<EventLoggerRegister>;

    static std::auto_ptr<EventLoggerRegister> s_destroyer;
    Q_DISABLE_COPY(EventLoggerRegister)
};

#endif
