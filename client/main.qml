import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Scroll")

    Button {
        id: button
        x: 136
        y: 243
        text: qsTr("login")
        onClicked: {
            netaccess.onLogin(txtUser.text, txtPass.text)
            console.log("enter user=" + txtUser.text + ":pass=" + txtPass.text)
        }
    }

    TextEdit {
        id: txtUser
        x: 136
        y: 141
        width: 80
        height: 20
        text: qsTr("10001")
        font.pixelSize: 12
    }

    TextEdit {
        id: txtPass
        x: 136
        y: 185
        width: 80
        height: 20
        text: qsTr("123")
        font.pixelSize: 12
    }

}
