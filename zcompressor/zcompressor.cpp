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

#include "zcompressor.h"

bool ZCompressor::open(QIODevice::OpenMode mode)
{
    if (!isOpen() && m_device && m_device->isOpen())
    {
        if (mode & QIODevice::WriteOnly)
            m_state = defInit(&m_strm, m_level, m_format);
        else if (mode & QIODevice::ReadOnly)
            m_state = infInit(&m_strm, m_format);
        else
            m_state = Z_ERRNO;

        if (m_state == Z_OK)
            QIODevice::open(mode);
    }

    return isOpen();
}

void ZCompressor::close()
{
    if (isOpen())
    {
        if (openMode() & QIODevice::WriteOnly)
        {
            QIODevice::close();
            if (!m_end)
                m_state = def(reinterpret_cast<unsigned char*>(0), 0, Z_FINISH);
            deflateEnd(&m_strm);
        }
        else
        {
            QIODevice::close();
            inflateEnd(&m_strm);
        }
    }
}

void ZCompressor::setDevice(QIODevice *device)
{
    if (!isOpen() && m_device != device)
    {
        if (m_device)
            disconnect(m_device, &QIODevice::readyRead, this, &ZCompressor::readyRead);

        m_device = device;
        connect(m_device, &QIODevice::readyRead, this, &ZCompressor::readyRead);
    }
}

qint64 ZCompressor::writeData(const char *data, qint64 len)
{
    if (!m_end)
    {        
        m_state = def(reinterpret_cast<unsigned char*>(const_cast<char*>(data)), len, Z_NO_FLUSH);
        if (m_state == Z_OK)
            return len;
        else
        {
            m_end = true;
            return -1;
        }
    }

    return -1;
}

int ZCompressor::def(unsigned char *data, qint64 length, int flush)
{
    int ret = Z_OK;
    //Potential truncation!
    m_strm.avail_in = static_cast<decltype(m_strm.avail_in)>(length);
    m_strm.next_in = data;

    do
    {
        m_strm.avail_out = CHUNK;
        m_strm.next_out = m_buffer.data();

        ret = deflate(&m_strm, flush);
        Q_ASSERT(ret != Z_STREAM_ERROR);

        qint64 have = CHUNK - m_strm.avail_out;
        if (m_device->write(reinterpret_cast<char*>(m_buffer.data()), have) != have)
        {
            ret = Z_ERRNO;
            setErrorString("error writing device");
            break;
        }
    }
    while (m_strm.avail_out == 0);
    Q_ASSERT(m_strm.avail_in == 0);

    return ret;
}

//static.
int ZCompressor::def(QIODevice *src, QIODevice *dest, int level, CompressFormat format)
{
    int ret, flush;
    qint64 have;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    z_stream strm;
    strm.zalloc = reinterpret_cast<decltype(strm.zalloc)>(Z_NULL);
    strm.zfree = reinterpret_cast<decltype(strm.zfree)>(Z_NULL);
    strm.opaque = reinterpret_cast<decltype(strm.opaque)>(Z_NULL);

    ret = defInit(&strm, level, format);
    if (ret != Z_OK)
        return ret;

    do
    {
        qint64 avail = src->read(reinterpret_cast<char*>(in), CHUNK);
        if (avail < 0)
        {
            deflateEnd(&strm);
            return Z_ERRNO;
        }
        //Potential truncation!
        strm.avail_in = static_cast<decltype(strm.avail_in)>(avail);

        flush = src->atEnd() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = reinterpret_cast<unsigned char*>(in);

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = deflate(&strm, flush);
            Q_ASSERT(ret != Z_STREAM_ERROR);

            have = CHUNK - strm.avail_out;
            if (dest->write(reinterpret_cast<char*>(out), have) != have)
            {
                deflateEnd(&strm);
                return Z_ERRNO;
            }
        }
        while (strm.avail_out == 0);
        Q_ASSERT(strm.avail_in == 0);
    }
    while (flush != Z_FINISH);
    Q_ASSERT(ret == Z_STREAM_END);

    deflateEnd(&strm);
    return Z_OK;
}

//static.
int ZCompressor::def(const QByteArray &src, QIODevice *dest, int level, CompressFormat format)
{
    int ret;
    unsigned long availOut = compressBound(static_cast<unsigned long>(src.size()));
    QScopedArrayPointer<unsigned char> out(new unsigned char[availOut]);
    QScopedArrayPointer<unsigned char> in(new unsigned char[src.size()]);

    z_stream strm;
    strm.zalloc = reinterpret_cast<decltype(strm.zalloc)>(Z_NULL);
    strm.zfree = reinterpret_cast<decltype(strm.zfree)>(Z_NULL);
    strm.opaque = reinterpret_cast<decltype(strm.opaque)>(Z_NULL);

    ret = defInit(&strm, level, format);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = static_cast<decltype(strm.avail_in)>(src.size());
    memcpy(in.data(), src.data(), static_cast<size_t>(src.size()));
    strm.next_in = in.data();
    strm.avail_out = static_cast<decltype(strm.avail_out)>(availOut);
    strm.next_out = out.data();

    do
    {
        ret = deflate(&strm, Z_FINISH);
        Q_ASSERT(ret != Z_STREAM_ERROR);

        qint64 have = static_cast<qint64>(availOut - strm.avail_out);
        if (dest->write(reinterpret_cast<char*>(out.data()), have) != have)
        {
            deflateEnd(&strm);
            return Z_ERRNO;
        }

    } while (strm.avail_out == 0);
    Q_ASSERT(strm.avail_in == 0);
    Q_ASSERT(ret == Z_STREAM_END);

    deflateEnd(&strm);
    return Z_OK;
}

qint64 ZCompressor::readData(char *data, qint64 maxlen)
{
    if (!m_end)
    {
        qint64 have;
        m_state = inf(reinterpret_cast<unsigned char*>(data), maxlen, have);
        if (m_state != Z_OK)
            m_end = true;

        return have;
    }

    return -1;
}

int ZCompressor::inf(unsigned char *data, qint64 length, qint64 &have)
{
    int ret = Z_OK;
    have = 0;

    //No update if out not full.
    m_strm.avail_out = static_cast<decltype(m_strm.avail_out)>(length);
    m_strm.next_out = data;

    do
    {
        if (m_strm.avail_in <= 0)
        {
            qint64 avail = m_device->read(reinterpret_cast<char*>(m_buffer.data()), CHUNK);
            if (avail < 0)
            {
                ret = Z_ERRNO;
                have = -1;
                setErrorString("error reading device");
                break;
            }
            //Potential truncation!
            m_strm.avail_in = static_cast<decltype(m_strm.avail_in)>(avail);

            if (avail == 0)
            {
                if (m_device->isSequential())
                    ret = Z_OK;
                else
                {
                    ret = Z_DATA_ERROR;
                    setErrorString("invalid end of compressed stream");
                }
                break;
            }

            m_strm.next_in = m_buffer.data();
        }

        ret = inflate(&m_strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);
        switch (ret)
        {
        case Z_BUF_ERROR:
            ret = Z_OK;
            return ret;
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
        [[clang::fallthrough]];
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            have = -1;
            setErrorString(m_strm.msg);
            return ret;
        }

        have = length - m_strm.avail_out;
    }
    while (m_strm.avail_out != 0 && ret != Z_STREAM_END);

    return ret;
}

//static.
int ZCompressor::inf(QIODevice *src, QIODevice *dest, CompressFormat format)
{
    int ret;
    qint64 have;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    z_stream strm;
    strm.zalloc = reinterpret_cast<decltype(strm.zalloc)>(Z_NULL);
    strm.zfree = reinterpret_cast<decltype(strm.zfree)>(Z_NULL);
    strm.opaque = reinterpret_cast<decltype(strm.opaque)>(Z_NULL);
    strm.avail_in = 0;
    strm.next_in = reinterpret_cast<decltype(strm.next_in)>(Z_NULL);

    ret = infInit(&strm, format);
    if (ret != Z_OK)
        return ret;

    do
    {
        qint64 avail = src->read(reinterpret_cast<char*>(in), CHUNK);
        if (avail < 0)
        {
            inflateEnd(&strm);
            return Z_ERRNO;
        }
        //Potential truncation!
        strm.avail_in = static_cast<decltype(strm.avail_in)>(avail);

        //End of file but not compressed stream.
        if (strm.avail_in == 0)
            break;

        strm.next_in = in;

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            Q_ASSERT(ret != Z_STREAM_ERROR);
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            [[clang::fallthrough]];
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                return ret;
            }

            have = CHUNK - strm.avail_out;
            if (dest->write(reinterpret_cast<char*>(out), have) != have)
            {
                inflateEnd(&strm);
                return Z_ERRNO;
            }
        }
        while (strm.avail_out == 0);
    }
    while(ret != Z_STREAM_END);

    inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

//static.
int ZCompressor::defInit(z_stream *strm, int level, CompressFormat format)
{
    strm->zalloc = reinterpret_cast<decltype(strm->zalloc)>(Z_NULL);
    strm->zfree = reinterpret_cast<decltype(strm->zfree)>(Z_NULL);
    strm->opaque = reinterpret_cast<decltype(strm->opaque)>(Z_NULL);
    strm->avail_in = 0;
    strm->next_in = reinterpret_cast<decltype(strm->next_in)>(Z_NULL);
    strm->avail_out = 0;
    strm->next_out = reinterpret_cast<decltype(strm->next_out)>(Z_NULL);

    switch (format)
    {
    case ZlibFormat:
        return deflateInit2(strm, level, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    case GzipFormat:
        return deflateInit2(strm, level, Z_DEFLATED, 30, 8, Z_DEFAULT_STRATEGY);
    case RawDeflateFormat:
        return deflateInit2(strm, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    }

    return Z_ERRNO;
}

//static.
int ZCompressor::infInit(z_stream *strm, CompressFormat format)
{
    strm->zalloc = reinterpret_cast<decltype(strm->zalloc)>(Z_NULL);
    strm->zfree = reinterpret_cast<decltype(strm->zfree)>(Z_NULL);
    strm->opaque = reinterpret_cast<decltype(strm->opaque)>(Z_NULL);
    strm->avail_in = 0;
    strm->next_in = reinterpret_cast<decltype(strm->next_in)>(Z_NULL);
    strm->avail_out = 0;
    strm->next_out = reinterpret_cast<decltype(strm->next_out)>(Z_NULL);

    switch (format)
    {
    case ZlibFormat:
        return inflateInit2(strm, MAX_WBITS);
    case GzipFormat:
        return inflateInit2(strm, 30);
    case RawDeflateFormat:
        return inflateInit2(strm, -MAX_WBITS);
    }

    return Z_ERRNO;
}
