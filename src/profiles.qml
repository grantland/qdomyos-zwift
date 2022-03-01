import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.1
import Qt.labs.folderlistmodel 2.15

ColumnLayout {
    MessageDialog {
        id: quitDialog
        title: "Profile loaded"
        text: "Would you like to quit?"
        informativeText: "You must quit and restart for changes to take effect."
        buttons: (MessageDialog.Yes | MessageDialog.No)
        onYesClicked: {
            restart()
        }
        onNoClicked: {
            quitDialog.close()
        }
    }

    MessageDialog {
        id: deleteDialog
        property string fileUrl
        title: "Delete profile"
        text: "Would you like to delete this profile?"
        buttons: (MessageDialog.Yes | MessageDialog.No)
        onYesClicked: {
            deleteSettings(fileUrl)
        }
        onNoClicked: {
            deleteDialog.close()
        }
    }

    AccordionElement {
        title: qsTr("Profiles")
        indicatRectColor: Material.color(Material.Grey)
        textColor: Material.color(Material.Grey)
        color: Material.backgroundColor
        accordionContent: ColumnLayout {
            ListView {
                id: list
                anchors.fill: parent
                FolderListModel {
                    id: folderModel
                    nameFilters: ["*.qzs"]
                    folder: "file://" + rootItem.getWritableAppDir() + 'profiles'
                    showDotAndDotDot: false
                    showDirs: true
                    sortReversed: true
                }
                model: folderModel
                delegate: Component {
                    Rectangle {
                        property alias textColor: fileTextBox.color
                        width: parent.width
                        height: 40
                        color: Material.backgroundColor
                        z: 1
                        Text {
                            id: fileTextBox
                            color: Material.color(Material.Grey)
                            font.pixelSize: Qt.application.font.pixelSize * 1.6
                            text: fileName.substring(0, fileName.length-4)
                        }
                        MouseArea {
                            anchors.fill: parent
                            z: 100
                            onClicked: {
                                console.log('onclicked ' + index+ " count "+list.count);
                                if (index == list.currentIndex) {
                                    let fileUrl = folderModel.get(list.currentIndex, 'fileUrl') || folderModel.get(list.currentIndex, 'fileURL');
                                    if (fileUrl) {
                                        saveProfile();
                                        loadSettings(fileUrl);
                                        quitDialog.visible = true
                                    }
                                }
                                else {
                                    if (list.currentItem)
                                        list.currentItem.textColor = Material.color(Material.Grey)
                                    list.currentIndex = index
                                }
                            }
                            onPressAndHold: {
                                console.log('onPressAndHold ' + index+ " count "+list.count);
                                deleteDialog.informativeText = folderModel.get(index, 'fileName').substring(0, fileName.length-4)
                                deleteDialog.fileUrl = folderModel.get(index, 'fileUrl') || folderModel.get(index, 'fileURL')
                                deleteDialog.visible = true
                            }
                        }
                    }
                }
                highlight: Rectangle {
                    color: Material.color(Material.Green)
                    z:3
                    radius: 5
                    opacity: 0.4
                    focus: true
                    /*Text {
                        anchors.centerIn: parent
                        text: 'Selected ' + folderModel.get(list.currentIndex, "fileName")
                        color: "white"
                    }*/
                }
                focus: true
                onCurrentItemChanged: {
                    let fileUrl = folderModel.get(list.currentIndex, 'fileUrl') || folderModel.get(list.currentIndex, 'fileURL');
                    if (fileUrl) {
                        list.currentItem.textColor = Material.color(Material.Yellow)
                        console.log(fileUrl + ' selected');
                    }
                }
            }
        }
    }
    spacing: 10

    Button {
        id: searchButton
        height: 50
        width: parent.width
        text: "Save Profile"
        Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
        onClicked: {
            console.log("folder is " + rootItem.getWritableAppDir() + 'profiles')
            saveProfile();
        }
        anchors {
            bottom: parent.bottom
        }
    }
}
