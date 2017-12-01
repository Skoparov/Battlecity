import QtQuick 2.3

Item
{
    id: tile
    width: modelData.width
    height: modelData.height

    Image
    {
        id: tileBackground
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
    }
}
