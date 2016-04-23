#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys

sys.path.append(sys.argv[1])

from PyQt5 import QtCore
from PyQt5 import QtWidgets

from PyKF5 import KItemModels

def main():
    app = QtWidgets.QApplication(sys.argv)

    stringListModel = QtCore.QStringListModel(["Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"]);

    selectionModel = QtCore.QItemSelectionModel()
    selectionModel.setModel(stringListModel)

    selectionProxy = KItemModels.KSelectionProxyModel()
    selectionProxy.setSelectionModel(selectionModel)
    selectionProxy.setSourceModel(stringListModel)

    assert(selectionProxy.rowCount() == 0)

    selectionModel.select(stringListModel.index(0, 0), QtCore.QItemSelectionModel.Select)

    assert(selectionProxy.rowCount() == 1)

if __name__ == '__main__':
    main()
