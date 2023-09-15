#pragma once

#include "kcolumnheadersmodel.h"
#include "knumbermodel.h"

#include <qqml.h>

struct KNumberModelForeign {
    Q_GADGET
    QML_FOREIGN(KNumberModel)
    QML_NAMED_ELEMENT(KNumberModel)
};

struct KColumnHeadersModelForeign {
    Q_GADGET
    QML_FOREIGN(KColumnHeadersModel)
    QML_NAMED_ELEMENT(KColumnHeadersModel)
};
