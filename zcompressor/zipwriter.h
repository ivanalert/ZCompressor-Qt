#ifndef ZIPWRITER_H
#define ZIPWRITER_H

#include "zipheader.h"
#include "zcompressor.h"

#include <QObject>
#include <QList>
#include <QDataStream>

class QBuffer;
class QByteArray;

class ZipWriter : public QObject
{
    Q_OBJECT

public:
    ZipWriter();
    ZipWriter(QIODevice *out);
    virtual ~ZipWriter();

    bool writeFile(const QString &name, const QByteArray &bytes);
    bool writeStartFile(const QString &name);
    bool writeBytes(const QByteArray &bytes);
    void writeEndFile();
    void writeEndArchive();

    void setDevice(QIODevice *device)
    {
        m_strm.setDevice(device);
        m_cmprs.close();
    }

    QIODevice* device() const
    {
        return m_strm.device();
    }

private:
    void appendLocalFileHeader(const ZipHeader &header);

    qint32 centralDirectorySize()
    {
        qint32 result = 0;
        for (int i = 0, size = m_headers.size(); i < size; ++i)
            result += 46 + m_headers.at(i).name().size();

        return result;
    }

    qint32 centralDirectoryOffset()
    {
        qint32 result = 0;
        for (int i = 0, size = m_headers.size(); i < size; ++i)
        {
            const ZipHeader header = m_headers.at(i);
            result += 30 + header.name().size() + header.compressedSize();
        }

        return result;
    }

    QDataStream m_strm;
    ZCompressor m_cmprs;
    QList<ZipHeader> m_headers;
};

#endif // ZIPWRITER_H
