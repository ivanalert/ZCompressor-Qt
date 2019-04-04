#ifndef ZCOMPRESSOR_H
#define ZCOMPRESSOR_H

#include <QIODevice>
#include <zlib.h>

class ZCompressor : public QIODevice
{
    Q_OBJECT

public:
    enum CompressFormat
    {
        ZlibFormat,
        GzipFormat,
        RawDeflateFormat
    };

    explicit ZCompressor(QObject *parent = nullptr);
    explicit ZCompressor(QIODevice *device, QObject *parent = nullptr);
    ~ZCompressor() override;

    // QIODevice interface
    bool open(OpenMode mode) override;
    void close() override;

    bool isSequential() const override
    {
        return true;
    }

    bool atEnd() const override
    {
        return m_end && QIODevice::atEnd();
    }

    qint64 bytesAvailable() const override
    {
        if (openMode() & QIODevice::ReadOnly)
        {
            qint64 result = QIODevice::bytesAvailable();
            if (result <= 0)
                result = m_device->bytesAvailable() + m_strm.avail_in;

            return result;
        }

        return 0;
    }

    qint64 bytesToWrite() const override
    {
        if (openMode() & QIODevice::WriteOnly)
        {
            qint64 result = QIODevice::bytesToWrite();
            if (result <= 0)
                result = m_device->bytesToWrite() + m_strm.avail_in;

            return result;
        }

        return 0;
    }

    static int def(QIODevice *src, QIODevice *dest, int level, CompressFormat format);
    static int def(const QByteArray &src, QIODevice *dest, int level, CompressFormat format);
    static int inf(QIODevice *src, QIODevice *dest, CompressFormat format);

    void setDevice(QIODevice *device);

    QIODevice* device() const
    {
        return m_device;
    }

    void setCompressLevel(int level)
    {
        m_level = level;
    }

    int compressLevel() const
    {
        return m_level;
    }

    void setCompressFormat(CompressFormat format)
    {
        m_format = format;
    }

    CompressFormat compressFormat() const
    {
        return m_format;
    }

    int state() const
    {
        return m_state;
    }

    unsigned long totalIn() const
    {
        return m_strm.total_in;
    }

    unsigned long totalOut() const
    {
        return m_strm.total_out;
    }

protected:
    // QIODevice interface
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    static int defInit(z_stream *strm, int level, CompressFormat format);
    static int infInit(z_stream *strm, CompressFormat format);

    static const unsigned CHUNK;

    int def(unsigned char *data, qint64 length, int flush);
    int inf(unsigned char *data, qint64 length, qint64 &have);

    QIODevice *m_device;
    z_stream m_strm;
    int m_level;
    CompressFormat m_format;
    int m_state;
    bool m_end;

    QScopedPointer<unsigned char, QScopedPointerPodDeleter> m_buffer;
};

#endif // ZCOMPRESSOR_H
