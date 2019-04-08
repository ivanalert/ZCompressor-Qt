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

#include "zipwriter.h"
#include "zcompressor.h"

#include <QDataStream>
#include <QBuffer>
#include <QDateTime>

ZipWriter::ZipWriter()
{
    m_strm.setByteOrder(QDataStream::LittleEndian);
    m_cmprs.setCompressFormat(ZCompressor::RawDeflateFormat);
    m_cmprs.setCompressLevel(8);
}

ZipWriter::ZipWriter(QIODevice *out)
    : m_strm(out)
{
    m_strm.setByteOrder(QDataStream::LittleEndian);
    m_cmprs.setCompressFormat(ZCompressor::RawDeflateFormat);
    m_cmprs.setCompressLevel(8);
}

ZipWriter::~ZipWriter()
{

}

void ZipWriter::appendLocalFileHeader(const ZipHeader &header)
{
    //Local file header.
    //Signature.
    m_strm << qint8(0x50);
    m_strm << qint8(0x4b);
    m_strm << qint8(0x03);
    m_strm << qint8(0x04);

    //Compress version.
    m_strm << qint16(0x14);
    //Flags.
    m_strm << qint16(0x0);
    //Compression 8 - deflate.
    m_strm << qint16(0x8);

    //Modification time.
    m_strm << header.time();
    //Modification date.
    m_strm << header.date();

    //CRC32.
    m_strm << header.crc32();

    //Compressed length.
    m_strm << header.compressedSize();
    //Uncompressed length.
    m_strm << header.uncompressedSize();
    //File name length.
    m_strm << header.nameSize();
    //Extra field length.
    m_strm << qint16(0x0);
    //File name.
    //Write only raw bytes (QDataStream::WriteRawData)!
    const QByteArray fileNameBytes = header.name();
    m_strm.writeRawData(fileNameBytes.data(), fileNameBytes.size());

    m_headers.append(header);
}

bool ZipWriter::writeFile(const QString &name, const QByteArray &bytes)
{
    QByteArray comprBytes;
    QBuffer buff(&comprBytes);
    buff.open(QIODevice::WriteOnly);
    if (ZCompressor::def(bytes, &buff, 8, ZCompressor::RawDeflateFormat) != Z_OK)
        return false;

    ZipHeader header(name, m_strm.device()->pos(),
                     crc32(0, (const unsigned char*)bytes.data(), bytes.size()), 0, bytes.size());
    header.setTime(QTime::currentTime());
    header.setDate(QDate::currentDate());
    header.setCompressedSize(comprBytes.size());
    appendLocalFileHeader(header);

    //Compressed bytes.
    m_strm.writeRawData(comprBytes.data(), comprBytes.size());
    return true;
}

bool ZipWriter::writeStartFile(const QString &name)
{
    m_cmprs.setDevice(m_strm.device());
    if (m_cmprs.open(QIODevice::WriteOnly))
    {
        ZipHeader header(name, m_strm.device()->pos());
        header.setTime(QTime::currentTime());
        header.setDate(QDate::currentDate());
        appendLocalFileHeader(header);

        return true;
    }

    return false;
}

bool ZipWriter::writeBytes(const QByteArray &bytes)
{
    int count = m_cmprs.write(bytes);
    if (count > 0)
    {
        ZipHeader &header = m_headers[m_headers.size() - 1];
        header.setCrc32(crc32(header.crc32(), (const unsigned char*)bytes.data(), bytes.size()));
        header.setUncompressedSize(header.uncompressedSize() + bytes.size());
    }

    return count != -1;
}

void ZipWriter::writeEndFile()
{
    m_cmprs.close();

    ZipHeader &header = m_headers[m_headers.size() - 1];
    header.setCompressedSize(m_cmprs.totalOut());

    QIODevice *dev = m_strm.device();
    dev->seek(header.offset() + 14);

    m_strm << header.crc32();
    m_strm << header.compressedSize();
    m_strm << header.uncompressedSize();

    dev->seek(dev->size());
}

void ZipWriter::writeEndArchive()
{
    for (int i = 0, size = m_headers.size(); i < size; ++i)
    {
        const ZipHeader header = m_headers.at(i);

        //Central directory.
        //Signature.
        m_strm << qint8(0x50);
        m_strm << qint8(0x4b);
        m_strm << qint8(0x01);
        m_strm << qint8(0x02);

        //Conmpress version.
        m_strm << qint16(0x14);
        //Decompress version.
        m_strm << qint16(0x14);

        //Flags.
        m_strm << qint16(0x0);

        //Compression 8 - deflate.
        m_strm << qint16(0x8);

        //Modification time.
        m_strm << header.time();
        //Modification date.
        m_strm << header.date();

        //CRC32.
        m_strm << header.crc32();

        //Compressed size.
        m_strm << header.compressedSize();
        //Uncompressed size.
        m_strm << header.uncompressedSize();

        //File name length.
        m_strm << header.nameSize();
        //Extra field length.
        m_strm << qint16(0x0);
        //File comment length.
        m_strm << qint16(0x0);
        //Disk start.
        m_strm << qint16(0x0);
        //Internal attribute 3rd bit - not used.
        m_strm << qint16(0x1);
        //External attribute.
        m_strm << qint32(0x20);
        //Offset of local header from start.
        m_strm << header.offset();
        //File name.
        const QByteArray nameBytes = header.name();
        m_strm.writeRawData(nameBytes.data(), nameBytes.size());
    }

    //End of central directory.
    //Signature.
    m_strm << qint8(0x50);
    m_strm << qint8(0x4b);
    m_strm << qint8(0x05);
    m_strm << qint8(0x06);

    //Disk number.
    m_strm << qint16(0x0);
    //Disk where central directory.
    m_strm << qint16(0x0);

    //Disk entries.
    m_strm << qint16(m_headers.size());
    //Total entries.
    m_strm << qint16(m_headers.size());

    //Size of central directory.
    m_strm << centralDirectorySize();

    //Offset of central directory from start.
    m_strm << centralDirectoryOffset();

    //Comment length.
    m_strm << qint16(0x0);

    m_headers.clear();
}
