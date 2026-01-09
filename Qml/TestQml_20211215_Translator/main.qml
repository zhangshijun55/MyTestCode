import QtQuick 2.12
import QtQml 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as Ctrl1
import QtQuick.Layouts 1.12
import Trans 1.0

Window {
    width: 860
    height: 640
    visible: true
    title: qsTr("Qt Translator")

    Column {
        x: 20
        y: 10
        spacing: 10
        // 第一行，切换按钮
        Row {
            spacing: 20
            RadioButton {
                text: "中文"
                checked: translator.language === Translator.ZH_CN
                onClicked: {
                    translator.language = Translator.ZH_CN
                }
            }
            RadioButton {
                text: "English"
                checked: translator.language === Translator.EN_US
                onClicked: {
                    translator.language = Translator.EN_US
                }
            }
        }
        // 第二行：文本
        Row {
            id: text_row
            spacing: 20
            Text {
                // 动态绑定且用 qsTr 括起来的字符串可以动态切换翻译文本
                text: qsTr("翻译")
            }
            Text {
                text: "翻译 " + qsTr("翻译")
            }
            Text {
                // 没有对 qsTr("翻译 %1") 进行翻译
                text: qsTr("翻译 %1").arg(qsTr("翻译"))
            }
            Component.onCompleted: {
                // createQmlObject 创建的对象，qsTr 找不到上下文没法翻译
                Qt.createQmlObject('import QtQuick 2.15; Text { text: qsTr("翻译") }', text_row)
                // 使用 qsTranslate 指定用 main.qml 的上下文
                Qt.createQmlObject('import QtQuick 2.15; Text { text: qsTranslate("main", "翻译") }', text_row)
                // createObject 创建的对象可以动态切换翻译文本
                text_comp.createObject(text_row)
            }
        }
        Component {
            id: text_comp
            Text {
                text: qsTr("翻译")
            }
        }
        // 第三行：Control组件
        Row {
            spacing: 20
            Button {
                text: qsTr("翻译")
            }
            Button {
                Component.onCompleted: {
                    // 直接赋值的字符串，即赋值前用了 qsTr，也不会动态切换翻译文本
                    // 使用 Qt.binding 进行动态绑定
                    text = Qt.binding(function() { return qsTr("翻译") })
                }
            }
            TextField {
                placeholderText: qsTr("翻译")
            }
            ComboBox {
                currentIndex: 2
                model: [qsTr("第一项"), qsTr("第二项"), qsTr("第三项")]
            }
            ComboBox {
                // 直接赋值的字符串，即赋值前用了 qsTr，也不会动态切换翻译文本
                Component.onCompleted: {
                    model = [qsTr("第一项"), qsTr("第二项"), qsTr("第三项")]
                    currentIndex = 2
                }
            }
        }
        // 第四行：Control组件
        Row {
            spacing: 20
            ComboBox {
                currentIndex: 2
                model: comboModel
            }
            ComboBox {
                currentIndex: 2
                model: listModel
                textRole: "txt"
            }
            ComboBox {
                currentIndex: 2
                model: listModel
                textRole: "trans"
            }
            ComboBox {
                currentIndex: 2
                model: ListModel {
                    ListElement {
                        text: qsTr("第一项")
                    }
                    ListElement {
                        text: qsTr("第二项")
                    }
                    ListElement {
                        text: qsTr("第三项")
                    }
                }
            }
            ComboBox {
                model: qml_combomodel
                ListModel {
                    id: qml_combomodel
                }
                Component.onCompleted: {
                    qml_combomodel.append({"text": qsTr("第一项")})
                    qml_combomodel.append({"text": qsTr("第二项")})
                    qml_combomodel.append({"text": qsTr("第三项")})
                    currentIndex = 2
                }
            }
        }
        // 第五行，列表
        ListView {
            width: 300
            height: contentHeight
            model: listModel
            spacing: 1
            delegate: Rectangle {
                height: 40
                width: ListView.view.width
                border.color: "black"
                Row {
                    x: 20
                    spacing: 20
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        text: qsTr("翻译")
                    }
                    Text {
                        text: model.txt
                    }
                    Text {
                        text: model.trans
                    }
                }
            }
        }
        // 第六行
        TabBar {
            id: tab_bar
            width: 300
            TabButton {
                text: qsTr("第一项")
            }
            TabButton {
                text: qsTr("第二项")
            }
            TabButton {
                text: qsTr("第三项")
            }
        }
        ListView {
            height: 40
            width: contentWidth
            orientation: ListView.Horizontal
            model: stack_layout.children
            spacing: 1
            // 不能绑定currentIndex，不然翻译后modelChanged就成了默认值
            // currentIndex: stack_layout.currentIndex
            delegate: Rectangle {
                width: 90
                height: ListView.view.height
                // color: ListView.isCurrentItem ? "gray" : "white"
                color: stack_layout.currentIndex === model.index ? "gray" : "white"
                border.color: "black"
                Text {
                    anchors.centerIn: parent
                    text: model.title
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: tab_bar.currentIndex = model.index
                }
            }
        }
        StackLayout {
            id: stack_layout
            width: 300
            height: 200
            currentIndex: tab_bar.currentIndex
            Rectangle {
                border.color: "black"
                property string title: qsTr("第一项")
                Text {
                    text: qsTr("第一项")
                }
            }
            Rectangle {
                border.color: "black"
                property string title: qsTr("第二项")
                Text {
                    text: qsTr("第二项")
                }
            }
            Rectangle {
                border.color: "black"
                property string title: qsTr("第三项")
                Text {
                    text: qsTr("第三项")
                }
            }
        }
    }

    Ctrl1.Calendar {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        // 默认值为Qt.locale()不能动态切换
        locale: translator.locale
    }
}
