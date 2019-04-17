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
    ZipHeader(const QString &name, quint32 offset);
    ZipHeader(const QString &name, quint32 offset, quint32 crc32, quint32 cSize, quint32 uSize);
    ZipHeader(const ZipHeader &other);
    ZipHeader& operator=(const ZipHeader &other);
    ZipHeader(ZipHeader &&other) noexcept;
    ZipHeader& operator=(ZipHeader &&other) noexcept;
    ~ZipHeader();

    //Name size limit is quint16 max value.
    void setName(const QString &name);
    QByteArray name() const noexcept;
    quint16 nameSize() const noexcept;

    void setTime(const QTime &time);
    quint16 time() const noexcept;

    void setDate(const QDate &date);
    quint16 date() const noexcept;

    void setOffset(quint32 offset) noexcept;
    quint32 offset() const noexcept;

    void setCrc32(quint32 crc32) noexcept;
    quint32 crc32() const noexcept;

    void setCompressedSize(quint32 size) noexcept;
    quint32 compressedSize() const noexcept;

    void setUncompressedSize(quint32 size) noexcept;
    quint32 uncompressedSize() const noexcept;

private:
    QSharedDataPointer<ZipHeaderData> m_data;
};

Q_DECLARE_METATYPE(ZipHeader)
Q_DECLARE_TYPEINFO(ZipHeader, Q_MOVABLE_TYPE);

#endif // ZIPHEADER_H
