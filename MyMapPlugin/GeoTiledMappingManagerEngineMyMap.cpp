#include "GeoTiledMappingManagerEngineMyMap.h"
#include "GeoTileFetcherMyMap.h"

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeofiletilecache_p.h>
#include <QFile>
#include <QDir>

QT_BEGIN_NAMESPACE
//删除文件夹(包括文件夹中的文件及目录)
bool DelDir(const QString &path)
{
    if (path.isEmpty()){
        return true;
    }

    QDir dir(path);
    if (!dir.exists()){
        dir.mkdir(dir.absolutePath());
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList();			 // 获取所有的文件信息
    //遍历文件信息
    foreach(QFileInfo file, fileList)
    {
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }
        else{				// 递归删除
            DelDir(file.absoluteFilePath());
        }
    }

//    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

GeoTiledMappingManagerEngineMyMap::GeoTiledMappingManagerEngineMyMap(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString)
{
    //地图视角相关设置，对应到 QML Map 类型的属性
    QGeoCameraCapabilities camera_caps;
    camera_caps.setMinimumZoomLevel(0.0);
    camera_caps.setMaximumZoomLevel(18.0);
    camera_caps.setSupportsBearing(true);
    camera_caps.setSupportsTilting(true);
    camera_caps.setMinimumTilt(0);
    camera_caps.setMaximumTilt(80);
    camera_caps.setMinimumFieldOfView(20.0);
    camera_caps.setMaximumFieldOfView(120.0);
    camera_caps.setOverzoomEnabled(true);
    setCameraCapabilities(camera_caps);

    //瓦片大小
    setTileSize(QSize(512,512));

    //瓦片获取，默认接口是通过网络请求获取
    //parameters 就是QML中设置的 PluginParameter
    GeoTileFetcherMyMap *tile_fetcher = new GeoTileFetcherMyMap(parameters, this);
    setTileFetcher(tile_fetcher);

    //瓦片缓存-这部分参考QtLocation的esri部分源码
    //如果没有用QGeoFileTileCache，默认加载地缓存不会释放，详情需分析他的源码
    QString cache_dir;
//    if(parameters.contains("mapPath")){
//        cache_dir = parameters.value("mapPath").toString() + QString("/mymapCache");
//    }else{
        cache_dir = QAbstractGeoTileCache::baseLocationCacheDirectory() + QString("mymapCache");
//    }
    if(DelDir(cache_dir)){
        qDebug()<<QString("缓存已清空");
    }else{
        qDebug()<<QString("清除缓存失败");
    }
    qDebug()<<cache_dir;
    //todo 自定义QGeoFileTileCache
    QGeoFileTileCache *tile_cache = new QGeoFileTileCache(cache_dir,this);
    //Disk cache setup -- defaults to ByteSize (old behavior)
    //tile_cache->setCostStrategyDisk(QGeoFileTileCache::ByteSize);
    //Memory cache setup -- defaults to ByteSize (old behavior)
    //tile_cache->setCostStrategyMemory(QGeoFileTileCache::ByteSize);
    //Texture cache setup -- defaults to ByteSize (old behavior)
    //tile_cache->setCostStrategyTexture(QGeoFileTileCache::ByteSize);
    const int cache_size = 512 * 1024 * 1024;
    tile_cache->setMaxDiskUsage(cache_size);
    setTileCache(tile_cache);

    m_prefetchStyle = QGeoTiledMap::NoPrefetching;
    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

GeoTiledMappingManagerEngineMyMap::~GeoTiledMappingManagerEngineMyMap()
{
}

QGeoMap *GeoTiledMappingManagerEngineMyMap::createMap()
{
    //todo 自定义QGeoTiledMap
    QGeoTiledMap *map = new QGeoTiledMap(this, nullptr);
    map->setPrefetchStyle(m_prefetchStyle);
    return map;
}
QT_END_NAMESPACE
