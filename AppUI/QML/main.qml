import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtPositioning 5.12
import QtLocation 5.12

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("My Qt Location (by: 龚建波)")

    Map {
        id: the_map
        anchors.fill: parent
        minimumZoomLevel: 1
        maximumZoomLevel: 3
        zoomLevel: 3
        center: QtPositioning.coordinate(0, 0) //178.594129933 , 0.1257984614

        plugin: Plugin {
            name: "mymap" //"esri" "mapbox" "osm" "here"

            //传递给插件的参数
            PluginParameter {
                name: "mapPath"
//                value: applicationDirPath+"/dianzi_gaode_ArcgisServerTiles/_alllayers"
//                value: applicationDirPath+"/Layers/_alllayers"
                value: "http://10.1.2.200:5010/asex/TiledMap/2023070401/Layers/_alllayers"
            }

            PluginParameter {
                name: "format"
                value: "png"
            }
        }

        //MapParameter { }
    }
}
