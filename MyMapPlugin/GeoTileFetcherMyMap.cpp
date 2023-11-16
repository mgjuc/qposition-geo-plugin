#include "GeoTileFetcherMyMap.h"
#include "GeoTiledMapReplyMyMap.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotilefetcher_p_p.h>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

QT_BEGIN_NAMESPACE

/**
 * @brief 计算瓦片起始编号
 * @param z
 * @param x
 * @param y
 */
void GeoTileFetcherMyMap::getTileXY(int z, int &x, int &y)
{
    if(info.empty()){
        getTielInfo();
        return;
    }
    double xMin,yMax, resolution[8];
    int size,originX,originY;
    QJsonValue dataValue = info.value("data");
    if(dataValue.type() != QJsonValue::Object || dataValue.isNull()) {
        qDebug()<<"解析Tileinfo失败";
        return;
    }
    QJsonObject dataObj = dataValue.toObject();
    QJsonValue originXValue = dataObj.value("originX");
    originX = originXValue.toString().toDouble();
    QJsonValue originYValue = dataObj.value("originY");
    originY = originYValue.toString().toDouble();
    QJsonValue sizeValue = dataObj.value("size");
    size = sizeValue.toString().toInt();
    QJsonValue xMinValue = dataObj.value("xMin");
    xMin = xMinValue.toString().toDouble();
    QJsonValue yMaxValue = dataObj.value("yMax");
    yMax = yMaxValue.toString().toDouble();
    QJsonValue resolutionValue = dataObj.value("resolution");
    if(resolutionValue.type() == QJsonValue::Array){
        QJsonArray resolutionArr = resolutionValue.toArray();
        for (int i = 0; i < resolutionArr.count(); i++) {
            QJsonValue res = resolutionArr.at(i);
            resolution[i] = res.toString().toDouble();
        }
    }
    //单张瓦片大小 米
    double r = resolution[z] * size;

    x += (-originX + xMin) / r;
    y += (originY - yMax) / r;


}
/**
 * @brief 获取瓦片信息
 */
void GeoTileFetcherMyMap::getTielInfo(){
    QString infourl = QString("http://10.1.2.200:5010/asex/map/GetTiledMapInfo");
    QNetworkRequest request;
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("MineId", "0");
    request.setUrl(infourl);
    QNetworkReply *reply = networkManager->get(request);
    if (!reply) {
        qDebug()<<QString("network error");
        return;
    }
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);

    // 请求错误处理
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&] {
        qDebug() << reply->errorString();
    });

    // 请求结束时删除 reply 释放内存
    // lambd表达式时reply必须是值引用
    connect(reply, &QNetworkReply::finished, [&, reply] {
//        qDebug()<<"labmda"<<sender()<<reply->url();
        int statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QJsonParseError err_rpt;
        QJsonDocument  doc = QJsonDocument::fromJson(reply->readAll(), &err_rpt);
        if( statuscode != 200 || err_rpt.error != QJsonParseError::NoError || doc.isNull()){
            qDebug() << "JSON格式错误";
            return;
        }
        info = doc.object();
        reply->deleteLater();
    });
}

GeoTileFetcherMyMap::GeoTileFetcherMyMap(
    const QVariantMap &parameters,
    QGeoMappingManagerEngine *parent)
    : QGeoTileFetcher(parent)
    , networkManager(new QNetworkAccessManager(this))
{
    //链接地址
    if(parameters.contains("mapPath")){
        //        mapUrl=QUrl::fromLocalFile(parameters.value("mapPath").toString()).toString();
        mapUrl=QUrl(parameters.value("mapPath").toString()).toString();
    }
    //瓦片图片格式
    if(parameters.contains("format")){
        format=parameters.value("format").toString();
    }
}

QGeoTiledMapReply *GeoTileFetcherMyMap::getTileImage(const QGeoTileSpec &spec)
{
    QNetworkRequest request;
    request.setRawHeader("Accept", "*/*");//设置网络请求头

    //request.setHeader(QNetworkRequest::UserAgentHeader, _userAgent);
    request.setRawHeader("MineId", "0");
    request.setUrl(getUrl(spec));
//    qDebug()<< "xyz="<<spec.x()<< spec.y()<<spec.zoom();
    qDebug()<<"GeoTileFetcherMyMap::getTileImage"<<request.url();

    QNetworkReply *reply = networkManager->get(request);
    return new GeoTiledMapReplyMyMap(reply, spec, format);
}

QString GeoTileFetcherMyMap::getUrl(const QGeoTileSpec &spec)
{
    //其实小写也可以，一般系统默认是不区分大小写的
    //用的arcgis 瓦片命名格式
    int x = spec.x(), y=spec.y();

    getTileXY(spec.zoom(), x, y);

    const QString xx=QString("%1").arg(x, 8,16,QLatin1Char('0')).toLower();
    const QString yy=QString("%1").arg(y, 8,16,QLatin1Char('0')).toLower();
    const QString zz=QString("%1").arg(spec.zoom(),2,10,QLatin1Char('0')).toLower();

    return QString("%1/L%2/R%3/C%4.%5").arg(mapUrl, zz, yy, xx, format);

}



QT_END_NAMESPACE
