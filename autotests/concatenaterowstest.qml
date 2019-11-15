import QtQuick 2.0
import org.kde.kitemmodels 1.0

KConcatenateRowsProxyModel {
    id: myModel
    ListModel {
        ListElement { name: "a" }
        ListElement { name: "b" }
    }
    ListModel {
        ListElement { name: "c" }
        ListElement { name: "d" }
    }
}


