#ifndef ZIPHEADER_H
#define ZIPHEADER_H

#include <QSharedDataPointer>
#include <QMetaType>

class QTime;
class QDate;
class ZipHeaderData;

class ZipHeader
{
public:
    ZipHeader();
    ZipHeader(const QString &name, qint64 offset);
    ZipHeader(const QString &name, qint64 offset, qint32 crc32, qint32 cSize, qint32 uSize);
    ZipHeader(const ZipHeader &other);
    ZipHeader& operator=(const ZipHeader &other);
    ~ZipHeader();

    void setName(const QString &name);
    QByteArray name() const;
    qint16 nameSize() const;

    void setTime(const QTime &time);
    qint16 time() const;

    void setDate(const QDate &date);
    qint16 date() const;

    void setOffset(qint32 offset);
    qint32 offset() const;

    void setCrc32(qint32 crc32);
    qint32 crc32() const;

    void setCompressedSize(qint32 size);
    qint32 compressedSize() const;

    void setUncompressedSize(qint32 size);
    qint32 uncompressedSize() const;

private:
    QSharedDataPointer<ZipHeaderData> m_data;
};

Q_DECLARE_METATYPE(ZipHeader)
Q_DECLARE_TYPEINFO(ZipHeader, Q_MOVABLE_TYPE);

#endif // ZIPHEADER_H
