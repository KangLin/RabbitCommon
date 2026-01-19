// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QIcon>
#include <QPixmap>
#include <QByteArray>
#include <QImage>

#include "rabbitcommon_export.h"

namespace RabbitCommon {

class RABBITCOMMON_EXPORT CIconUtils
{
public:
    // 将 QByteArray 转换为 QIcon
    static QIcon byteArrayToIcon(const QByteArray &data);

    // 将 QByteArray 转换为 QPixmap
    static QPixmap byteArrayToPixmap(const QByteArray &data);

    // 将 QByteArray 转换为 QImage
    static QImage byteArrayToImage(const QByteArray &data);

    // 将 QIcon 转换为 QByteArray
    static QByteArray iconToByteArray(const QIcon &icon, const char *format = "PNG",
                                      const QSize &size = QSize(32, 32));

    // 将 QPixmap 转换为 QByteArray
    static QByteArray pixmapToByteArray(const QPixmap &pixmap, const char *format = "PNG");

    // 将 QImage 转换为 QByteArray
    static QByteArray imageToByteArray(const QImage &image, const char *format = "PNG");

    // 检测图标数据类型
    static QString detectIconType(const QByteArray &data);

    // 获取图标信息
    static QSize getIconSize(const QByteArray &data);
    static QString getIconFormat(const QByteArray &data);
    static int getIconByteSize(const QByteArray &data);

    // 优化图标数据（调整大小、压缩等）
    static QByteArray optimizeIconData(const QByteArray &data,
                                       const QSize &maxSize = QSize(32, 32),
                                       int quality = 85);

    // 创建默认/占位图标
    static QIcon createDefaultIcon();
    static QIcon createDefaultIconForUrl(const QString &url);
    static QIcon createDefaultIconForDomain(const QString &domain);

    // 图标哈希
    static QString hashIconData(const QByteArray &data);
    static QString hashIconUrl(const QString &url);

    // 验证图标数据
    static bool isValidIconData(const QByteArray &data);

private:
    static QIcon createColoredIcon(const QColor &color, const QSize &size = QSize(16, 16));
    static QColor getColorForDomain(const QString &domain);
};

} // namespace RabbitCommon

