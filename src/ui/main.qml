import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2

ApplicationWindow {
    title: qsTr("Match")
    width: 995
    height: 735
    visible: true
    id: matchWindow

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&New Budget");
                onTriggered: {
                    var component = Qt.createComponent("newbudget.qml")
                    var win = component.createObject(matchWindow)
                    win.show()
                }
            }
            MenuItem {
                text: qsTr("&Open Budget")
            }
           MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
        Menu {
            title: qsTr("&Accounts")
            MenuItem {
               text: qsTr("&New Account")
               onTriggered: {
                   var component = Qt.createComponent("newaccount.qml")
                   var win = component.createObject(matchWindow, {updateList: mainSidebar})
                   win.show()
               }
           }
            MenuItem {
                text: qsTr("&Account Settings")
            }
        }
    }

    Sidebar {
        id: mainSidebar
        width: 225
        height: 725
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        targetLoader: contentLoader
    }

    Loader {
        id: contentLoader
        anchors.left: parent.left
        anchors.leftMargin: 225
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        source: "BudgetView.qml"
    }
}
