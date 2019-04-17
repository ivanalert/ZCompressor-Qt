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

#include "zipheader.h"

#include <QSharedData>
#include <QDateTime>

class ZipHeaderData : public QSharedData
{
public:
    ZipHeaderData() = default;

    ZipHeaderData(const QString &nameArg, quint32 offsetArg)
        : name(nameArg.toUtf8()), offset(offsetArg)
    {

    }

    ZipHeaderData(const QString &nameArg, quint32 offsetArg, quint32 crc32Arg, quint32 cSizeArg,
                  quint32 uSizeArg)
        : name(nameArg.toUtf8()), offset(offsetArg), crc32(crc32Arg), cSize(cSizeArg),
          uSize(uSizeArg)
    {

    }

    ~ZipHeaderData() = default;

    QByteArray name;
    quint32 offset{0};
    quint16 time{0};
    quint16 date{0};
    quint32 crc32{0};
    quint32 cSize{0};
    quint32 uSize{0};
};

ZipHeader::ZipHeader()
    : m_data(new ZipHeaderData)
{

}

ZipHeader::ZipHeader(const QString &name, quint32 offset)
    : m_data(new ZipHeaderData(name, offset))

{

}

ZipHeader::ZipHeader(const QString &name, quint32 offset, quint32 crc32, quint32 cSize,
                     quint32 uSize)
    : m_data(new ZipHeaderData(name, offset, crc32, cSize, uSize))
{

}

ZipHeader::ZipHeader(const ZipHeader &other)
    : m_data(other.m_data)
{

}

ZipHeader& ZipHeader::operator=(const ZipHeader &other)
{
    m_data = other.m_data;
    return *this;
}

ZipHeader::ZipHeader(ZipHeader &&other) noexcept
    : m_data(std::move(other.m_data))
{

}

ZipHeader &ZipHeader::operator=(ZipHeader &&other) noexcept
{
    m_data = std::move(other.m_data);
    return *this;
}

ZipHeader::~ZipHeader()
{

}

void ZipHeader::setName(const QString &name)
{
    m_data->name = name.toUtf8();
}

QByteArray ZipHeader::name() const noexcept
{
    return m_data->name;
}

quint16 ZipHeader::nameSize() const noexcept
{
    return static_cast<quint16>(m_data->name.size());
}

void ZipHeader::setTime(const QTime &time)
{
    //First 5 bits - second divided by 2.
    m_data->time = static_cast<quint16>((time.second() / 2));
    //Next 6 bits - minute.
    m_data->time |= time.minute() << 5;
    //Next 5 bits - hour.
    m_data->time |= time.hour() << 11;
}

quint16 ZipHeader::time() const noexcept
{
    return m_data->time;
}

void ZipHeader::setDate(const QDate &date)
{
    //First 5 bits - day.
    m_data->date = static_cast<quint16>(date.day());
    //Next 4 bits - month.
    m_data->date |= date.month() << 5;
    //Next 7 bits - year.
    m_data->date |= (date.year() - 1980) << 9;
}

quint16 ZipHeader::date() const noexcept
{
    return m_data->date;
}

void ZipHeader::setOffset(quint32 offset) noexcept
{
    m_data->offset = offset;
}

quint32 ZipHeader::offset() const noexcept
{
    return m_data->offset;
}

void ZipHeader::setCrc32(quint32 crc32) noexcept
{
    m_data->crc32 = crc32;
}

quint32 ZipHeader::crc32() const noexcept
{
    return m_data->crc32;
}

void ZipHeader::setCompressedSize(quint32 size) noexcept
{
    m_data->cSize = size;
}

quint32 ZipHeader::compressedSize() const noexcept
{
    return m_data->cSize;
}

void ZipHeader::setUncompressedSize(quint32 size) noexcept
{
    m_data->uSize = size;
}

quint32 ZipHeader::uncompressedSize() const noexcept
{
    return m_data->uSize;
}
