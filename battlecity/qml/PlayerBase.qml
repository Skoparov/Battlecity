import QtQuick 2.3

Item
{
    id: playerBase
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: playerBaseImage
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
    }
}
