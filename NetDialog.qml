import QtQuick 2.1
import Enginio 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: main
    signal ok(string a,string p);
    anchors.fill: parent
    anchors.margins: 3
    spacing: 3

    Text {
        text: "IP Address"
    }

    TextField {
        id: adr
        objectName: "adr"
        width:  parent.width
        height: parent.height/4
    }

    Text {
        text: "IP Port"
    }

    TextField {
        id: port
        objectName: "port"
        width:  parent.width
        height: parent.height/4
    }

    Button {
        id: proccessButton
        width:  parent.width
        height: parent.height/4
        text: "Ok"
        onClicked: ok(adr.text,port.text)
    }

    Button {
        id: cancelButton
        width:  parent.width
        height: parent.height/4
        text: "cancel"
        onClicked: ok("","")
    }

}
