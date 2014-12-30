import QtQuick 2.1
import Enginio 1.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.2

ColumnLayout {
    id: main
    signal ok(string a,string p);
    anchors.fill: parent
    anchors.margins: 3
    spacing: 3

    Text {
        text: "IP Address"
        font.pixelSize: parent.height/10
    }

    TextField {
        id: adr
        objectName: "adr"
        width:  parent.width
        height: parent.height/4
        font.pixelSize: parent.height/8
    }

    Text {
        text: "IP Port"
        font.pixelSize: parent.height/10
    }

    TextField {
        id: port
        objectName: "port"
        width:  parent.width
        height: parent.height/4
        font.pixelSize: parent.height/8
    }

    RowLayout {
        Button {
            id: proccessButton
            text: "Ok"
            onClicked: ok(adr.text,port.text)
        }
        Button {
            id: cancelButton
            text: "Cancel"
            onClicked: ok("","")
        }
    }
}
