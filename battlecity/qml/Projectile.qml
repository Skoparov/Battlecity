import QtQuick 2.3

Item
{
    id: projectile
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: projectile_image
        anchors.centerIn: parent
        width: modelData.width
        height: modelData.height
        source: modelData.image_path
        rotation: modelData.rotation
    }
}

