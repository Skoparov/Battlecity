import QtQuick 2.3

Item
{
    id: frag
    width: modelData.width
    height: modelData.height

    Image
    {
        id: frag_image
        anchors.centerIn: parent
        source: modelData.image_path
        visible: modelData.visible
    }
}

