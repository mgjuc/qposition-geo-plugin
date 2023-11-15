#pragma once
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QNetworkAccessManager>
#include <QJsonObject>

QT_BEGIN_NAMESPACE

/**
 * @brief 请求瓦片数据
 * @author 龚建波
 * @date 2021-07-03
 * @details
 * 在 engine 构造时构造，
 * 根据 QML 中的 parameters 初始化
 */
class GeoTileFetcherMyMap : public QGeoTileFetcher
{
    Q_OBJECT
public:
    GeoTileFetcherMyMap(
            const QVariantMap &parameters,
            QGeoMappingManagerEngine *parent);

private:
    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec) override;
    QString getUrl(const QGeoTileSpec &spec) const;
    void getTileXY(int z, int &x, int &y) const;
private:
    QString mapUrl;
    QString format{"png"};
    QNetworkAccessManager* networkManager;
    QJsonObject* info = nullptr;
    void getTielInfo(int z, int &x, int &y) const;
};
QT_END_NAMESPACE
