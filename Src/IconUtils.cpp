// Author: Kang Lin <kl222@126.com>

#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QSvgRenderer>
#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QImageReader>
#include <QBuffer>
#include "IconUtils.h"

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Icon.Utils")
QIcon CIconUtils::byteArrayToIcon(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QIcon();
    }

    // 尝试直接加载为 QIcon
    QIcon icon;
    QPixmap pixmap;

    // 方法1: 尝试从数据加载
    if (pixmap.loadFromData(data)) {
        icon.addPixmap(pixmap);
        return icon;
    }

    // 方法2: 通过 QImage 中转
    QImage image = byteArrayToImage(data);
    if (!image.isNull()) {
        icon.addPixmap(QPixmap::fromImage(image));
        return icon;
    }

    // 方法3: 尝试不同的格式
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    if (reader.canRead()) {
        image = reader.read();
        if (!image.isNull()) {
            icon.addPixmap(QPixmap::fromImage(image));
            return icon;
        }
    }

    // 方法4: 尝试检测并转换格式
    QString iconType = detectIconType(data);
    if (iconType == "ico") {
        // ICO 格式需要特殊处理
        QList<QSize> sizes;
        sizes << QSize(16, 16) << QSize(32, 32) << QSize(48, 48) << QSize(64, 64);

        for (const QSize &size : sizes) {
            QImage image = byteArrayToImage(data);
            if (!image.isNull()) {
                QPixmap pixmap = QPixmap::fromImage(image.scaled(size,
                                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));
                icon.addPixmap(pixmap);
            }
        }
    }

    if (!icon.isNull()) {
        return icon;
    }

    qCritical(log) << "Failed to convert byte array to icon, size:" << data.size();
    return QIcon();
}

QPixmap CIconUtils::byteArrayToPixmap(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QPixmap();
    }

    QPixmap pixmap;

    // 直接加载
    if (pixmap.loadFromData(data)) {
        return pixmap;
    }

    // 通过 QImage 中转
    QImage image = byteArrayToImage(data);
    if (!image.isNull()) {
        return QPixmap::fromImage(image);
    }

    // 尝试不同的格式
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);
    if (reader.canRead()) {
        image = reader.read();
        if (!image.isNull()) {
            return QPixmap::fromImage(image);
        }
    }

    qCritical(log) << "Failed to convert byte array to pixmap";
    return QPixmap();
}

QImage CIconUtils::byteArrayToImage(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QImage();
    }

    QImage image;

    // 直接加载
    if (image.loadFromData(data)) {
        return image;
    }

    // 尝试不同的格式
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);

    if (!reader.canRead()) {
        // 尝试自动检测格式
        QString format = detectIconType(data);
        if (!format.isEmpty()) {
            reader.setFormat(format.toUtf8());
        }
    }

    if (reader.canRead()) {
        image = reader.read();
        if (!image.isNull()) {
            return image;
        }
    }

    // 最后尝试：如果是 SVG
    if (data.startsWith("<?xml") || data.contains("<svg")) {
        QSvgRenderer renderer(data);
        if (renderer.isValid()) {
            image = QImage(32, 32, QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            renderer.render(&painter);
            return image;
        }
    }

    qCritical(log) << "Failed to convert byte array to image";
    return QImage();
}

QByteArray CIconUtils::iconToByteArray(const QIcon &icon, const char *format,
                                      const QSize &size)
{
    if (icon.isNull()) {
        return QByteArray();
    }

    // 获取最适合的 pixmap
    QPixmap pixmap = icon.pixmap(size.isValid() ? size : QSize(32, 32));
    if (pixmap.isNull()) {
        // 尝试获取任意大小的 pixmap
        QList<QSize> availableSizes = icon.availableSizes();
        if (!availableSizes.isEmpty()) {
            pixmap = icon.pixmap(availableSizes.first());
        }
    }

    if (pixmap.isNull()) {
        return QByteArray();
    }

    return pixmapToByteArray(pixmap, format);
}

QByteArray CIconUtils::pixmapToByteArray(const QPixmap &pixmap, const char *format)
{
    if (pixmap.isNull()) {
        return QByteArray();
    }

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);

    if (pixmap.save(&buffer, format)) {
        return data;
    }

    // 如果指定格式失败，尝试 PNG
    if (strcmp(format, "PNG") != 0) {
        buffer.close();
        data.clear();
        buffer.open(QIODevice::WriteOnly);
        if (pixmap.save(&buffer, "PNG")) {
            return data;
        }
    }

    qCritical(log) << "Failed to convert pixmap to byte array";
    return QByteArray();
}

QByteArray CIconUtils::imageToByteArray(const QImage &image, const char *format)
{
    if (image.isNull()) {
        return QByteArray();
    }

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);

    if (image.save(&buffer, format)) {
        return data;
    }

    // 如果指定格式失败，尝试 PNG
    if (strcmp(format, "PNG") != 0) {
        buffer.close();
        data.clear();
        buffer.open(QIODevice::WriteOnly);
        if (image.save(&buffer, "PNG")) {
            return data;
        }
    }

    qWarning() << "Failed to convert image to byte array";
    return QByteArray();
}

QString CIconUtils::detectIconType(const QByteArray &data)
{
    if (data.size() < 8) {
        return "unknown";
    }

    // 检测常见的图像格式
    if (data.startsWith("\x89PNG\r\n\x1a\n")) {
        return "png";
    } else if (data.startsWith("GIF8")) {
        return "gif";
    } else if (data.startsWith("\xff\xd8")) {
        return "jpg";
    } else if (data.startsWith("BM")) {
        return "bmp";
    } else if (data.startsWith("\x00\x00\x01\x00")) {
        return "ico";
    } else if (data.startsWith("\x00\x00\x02\x00")) {
        return "cur";
    } else if (data.startsWith("RIFF") && data.mid(8, 4) == "WEBP") {
        return "webp";
    } else if (data.startsWith("<svg") || data.contains("xmlns=\"http://www.w3.org/2000/svg\"")) {
        return "svg";
    } else if (data.startsWith("<?xml")) {
        // 可能是 SVG
        if (data.contains("<svg") || data.contains("svg")) {
            return "svg";
        }
    }

    return "unknown";
}

QSize CIconUtils::getIconSize(const QByteArray &data)
{
    QImage image = byteArrayToImage(data);
    if (!image.isNull()) {
        return image.size();
    }

    // 对于 ICO 文件，可能需要特殊处理
    if (detectIconType(data) == "ico" && data.size() >= 22) {
        // ICO 文件头格式
        // 偏移量: 6 = 图像数量，接着是每个图像的目录项
        // 每个目录项: 1字节宽度，1字节高度，...
        int numImages = static_cast<unsigned char>(data[4]) + (static_cast<unsigned char>(data[5]) << 8);
        if (numImages > 0 && data.size() >= 6 + numImages * 16) {
            // 获取第一个图像的尺寸
            int width = static_cast<unsigned char>(data[6]);
            int height = static_cast<unsigned char>(data[7]);
            if (width == 0) width = 256;
            if (height == 0) height = 256;
            return QSize(width, height);
        }
    }

    return QSize();
}

QString CIconUtils::getIconFormat(const QByteArray &data)
{
    return detectIconType(data);
}

int CIconUtils::getIconByteSize(const QByteArray &data)
{
    return data.size();
}

QByteArray CIconUtils::optimizeIconData(const QByteArray &data,
                                       const QSize &maxSize,
                                       int quality)
{
    if (data.isEmpty()) {
        return data;
    }

    // 检测如果是 SVG，不需要优化
    QString format = detectIconType(data);
    if (format == "svg") {
        return data;  // SVG 是矢量格式，保持原样
    }

    // 转换为图像
    QImage image = byteArrayToImage(data);
    if (image.isNull()) {
        return data;
    }

    // 调整大小（如果太大）
    if (!maxSize.isNull() && (image.width() > maxSize.width() || image.height() > maxSize.height())) {
        image = image.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 转换为 PNG 并压缩
    QByteArray optimized;
    QBuffer buffer(&optimized);
    buffer.open(QIODevice::WriteOnly);

    // 对于 PNG，质量参数无效，但我们可以尝试压缩
    if (format == "png") {
        // PNG 格式，保持透明度
        image.save(&buffer, "PNG");
    } else {
        // 其他格式转换为 PNG
        image.save(&buffer, "PNG", quality);
    }

    // 如果优化后的数据比原始数据大，返回原始数据
    if (optimized.size() >= data.size()) {
        return data;
    }

    return optimized;
}

QIcon CIconUtils::createDefaultIcon()
{
    // 创建一个简单的默认图标
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆角矩形
    painter.setBrush(QColor(100, 149, 237));  // 玉米花色
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, 16, 16, 3, 3);

    // 绘制字母 "W" (Web)
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "W");

    QIcon icon(pixmap);
    return icon;
}

QIcon CIconUtils::createDefaultIconForUrl(const QString &url)
{
    if (url.isEmpty()) {
        return createDefaultIcon();
    }

    QUrl qurl(url);
    if (qurl.isValid()) {
        QString domain = qurl.host();
        if (domain.startsWith("www.")) {
            domain = domain.mid(4);
        }
        return createDefaultIconForDomain(domain);
    }

    return createDefaultIcon();
}

QIcon CIconUtils::createDefaultIconForDomain(const QString &domain)
{
    if (domain.isEmpty()) {
        return createDefaultIcon();
    }

    // 根据域名首字母或哈希值生成颜色
    QColor color = getColorForDomain(domain);
    QIcon icon = createColoredIcon(color);

    // 添加域名首字母
    QString firstLetter = domain.left(1).toUpper();

    QPixmap pixmap = icon.pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, firstLetter);

    icon.addPixmap(pixmap);
    return icon;
}

QColor CIconUtils::getColorForDomain(const QString &domain)
{
    if (domain.isEmpty()) {
        return QColor(100, 149, 237);  // 默认颜色
    }

    // 使用哈希函数生成确定性颜色
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(domain.toUtf8());
    QByteArray hashResult = hash.result();

    // 从哈希值生成颜色
    int r = static_cast<unsigned char>(hashResult[0]);
    int g = static_cast<unsigned char>(hashResult[1]);
    int b = static_cast<unsigned char>(hashResult[2]);

    // 确保颜色不太暗或太亮
    r = qMax(50, qMin(200, r));
    g = qMax(50, qMin(200, g));
    b = qMax(50, qMin(200, b));

    return QColor(r, g, b);
}

QIcon CIconUtils::createColoredIcon(const QColor &color, const QSize &size)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆形
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(1, 1, size.width() - 2, size.height() - 2);

    QIcon icon(pixmap);
    return icon;
}

QString CIconUtils::hashIconData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data);
    return QString::fromLatin1(hash.result().toHex());
}

QString CIconUtils::hashIconUrl(const QString &url)
{
    if (url.isEmpty()) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(url.toUtf8());
    return QString::fromLatin1(hash.result().toHex());
}

bool CIconUtils::isValidIconData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return false;
    }

    // 尝试加载为图像
    QImage image = byteArrayToImage(data);
    if (!image.isNull()) {
        return true;
    }

    // 检查是否为有效的 SVG
    if (data.contains("<svg") || (data.startsWith("<?xml") && data.contains("svg"))) {
        QSvgRenderer renderer(data);
        return renderer.isValid();
    }

    return false;
}

} //namespace RabbitCommon
