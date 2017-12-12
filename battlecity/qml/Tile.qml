import QtQuick 2.3

Item
{
    id: tile
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: tile_background
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
    }
}
