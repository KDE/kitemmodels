/*
    Copyright (c) 2015 Stephen Kelly <steveire@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

import QtQuick 2.4

import KF5ItemModels 1.0

Item {
  width: 600
  height: 500

  Component {
    id: labelledSelectionView

    Column {
      id: column
      width : 200
      property var filterBehavior
      property var filterBehaviorName
      Text {
        id: label
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        font.bold: true
        font.pointSize: 10
        text: column.filterBehaviorName
      }
      SelectionProxyModel {
        id: selection
        sourceModel: _model
        selectionModel: _selectionModel
        filterBehavior: column.filterBehavior
      }
      Rectangle {
        height: parent.height - label.height
        width: parent.width
        border.width : 1
        border.color: "black"
        radius: 5
        ListView {
          width: parent.width
          height: parent.height
          y: 5
          x: 5
          model: selection
          delegate: Rectangle {
            x: 1
            y: 1
            height: 30
            width: 100
            color: model.index % 2 == 0 ? "lightsteelblue" : "white"
            Text {
              x: 5
              y: 5
              text: model.display
            }
          }
        }
      }
    }
  }

  Loader {
    id: loaderExactSelection
    width : 200
    height: 300
    sourceComponent: labelledSelectionView
    Binding {
      target: loaderExactSelection.item
      property: "filterBehavior"
      value: SelectionProxyModel.ExactSelection
      when: loaderExactSelection.status == Loader.Ready
    }
    Binding {
      target: loaderExactSelection.item
      property: "filterBehaviorName"
      value: "ExactSelection"
      when: loaderExactSelection.status == Loader.Ready
    }
  }

  Loader {
    id: loaderChildrenOfExactSelection
    x: 200
    width : 200
    height: 300
    sourceComponent: labelledSelectionView
    Binding {
      target: loaderChildrenOfExactSelection.item
      property: "filterBehavior"
      value: SelectionProxyModel.ChildrenOfExactSelection
      when: loaderChildrenOfExactSelection.status == Loader.Ready
    }
    Binding {
      target: loaderChildrenOfExactSelection.item
      property: "filterBehaviorName"
      value: "ChildrenOfExactSelection"
      when: loaderChildrenOfExactSelection.status == Loader.Ready
    }
  }

  Loader {
    id: loaderSubTreeRoots
    x: 400
    width : 200
    height: 300
    sourceComponent: labelledSelectionView
    Binding {
      target: loaderSubTreeRoots.item
      property: "filterBehavior"
      value: SelectionProxyModel.SubTreeRoots
      when: loaderSubTreeRoots.status == Loader.Ready
    }
    Binding {
      target: loaderSubTreeRoots.item
      property: "filterBehaviorName"
      value: "SubTreeRoots"
      when: loaderSubTreeRoots.status == Loader.Ready
    }
  }

}
