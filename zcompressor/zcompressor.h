/*
    This file is part of ZCompressor.

    ZCompressor is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

    explicit ZCompressor(QObject *parent = nullptr)
        : QIODevice(parent), m_buffer(reinterpret_cast<unsigned char*>(malloc(CHUNK)))
    {

    }

    explicit ZCompressor(QIODevice *device, QObject *parent = nullptr)
        : QIODevice(parent), m_device(device),
          m_buffer(reinterpret_cast<unsigned char*>(malloc(CHUNK)))
    {
        connect(m_device, &QIODevice::readyRead, this, &ZCompressor::readyRead);
    }

    ~ZCompressor() override
    {
        close();
    }

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

    QIODevice* device() const noexcept
    {
        return m_device;
    }

    void setCompressLevel(int level) noexcept
    {
        m_level = level;
    }

    int compressLevel() const noexcept
    {
        return m_level;
    }

    void setCompressFormat(CompressFormat format) noexcept
    {
        m_format = format;
    }

    CompressFormat compressFormat() const noexcept
    {
        return m_format;
    }

    int state() const noexcept
    {
        return m_state;
    }

    unsigned long totalIn() const noexcept
    {
        return m_strm.total_in;
    }

    unsigned long totalOut() const noexcept
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

    static const unsigned CHUNK{16384};

    int def(unsigned char *data, qint64 length, int flush);
    int inf(unsigned char *data, qint64 length, qint64 &have);

    QIODevice *m_device{nullptr};
    z_stream m_strm;
    int m_level{6};
    CompressFormat m_format{ZlibFormat};
    int m_state{Z_OK};
    bool m_end{false};

    QScopedPointer<unsigned char, QScopedPointerPodDeleter> m_buffer;
};

#endif // ZCOMPRESSOR_H
