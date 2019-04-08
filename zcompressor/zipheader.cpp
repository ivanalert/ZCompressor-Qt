#include "zipheader.h"

#include <QSharedData>
#include <QDateTime>

class ZipHeaderData : public QSharedData
{
public:
    ZipHeaderData()
        : offset(0), time(0), date(0), crc32(0), cSize(0), uSize(0)
    {

    }

    ZipHeaderData(const QString &nameArg, qint32 offsetArg)
        : name(nameArg.toUtf8()), offset(offsetArg), time(0), date(0), crc32(0), cSize(0), uSize(0)
    {

    }

    ZipHeaderData(const QString &nameArg, qint32 offsetArg, qint32 crc32Arg, qint32 cSizeArg,
                  qint32 uSizeArg)
        : name(nameArg.toUtf8()), offset(offsetArg), time(0), date(0), crc32(crc32Arg),
          cSize(cSizeArg), uSize(uSizeArg)
    {

    }

    virtual ~ZipHeaderData()
    {

    }

    QByteArray name;
    qint32 offset;
    qint16 time;
    qint16 date;
    qint32 crc32;
    qint32 cSize;
    qint32 uSize;
};

ZipHeader::ZipHeader()
    : m_data(new ZipHeaderData)
{

}

ZipHeader::ZipHeader(const QString &name, qint64 offset)
    : m_data(new ZipHeaderData(name, offset))

{

}

ZipHeader::ZipHeader(const QString &name, qint64 offset, qint32 crc32, qint32 cSize, qint32 uSize)
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

ZipHeader::~ZipHeader()
{

}

void ZipHeader::setName(const QString &name)
{
    m_data->name = name.toUtf8();
}

QByteArray ZipHeader::name() const
{
    return m_data->name;
}

qint16 ZipHeader::nameSize() const
{
    return m_data->name.size();
}

void ZipHeader::setTime(const QTime &time)
{
    //First 5 bits - second divided by 2.
    m_data->time = (time.second() / 2);
    //Next 6 bits - minute.
    m_data->time |= time.minute() << 5;
    //Next 5 bits - hour.
    m_data->time |= time.hour() << 11;
}

qint16 ZipHeader::time() const
{
    return m_data->time;
}

void ZipHeader::setDate(const QDate &date)
{
    //First 5 bits - day.
    m_data->date = date.day();
    //Next 4 bits - month.
    m_data->date |= date.month() << 5;
    //Next 7 bits - year.
    m_data->date |= (date.year() - 1980) << 9;
}

qint16 ZipHeader::date() const
{
    return m_data->date;
}

void ZipHeader::setOffset(qint32 offset)
{
    m_data->offset = offset;
}

qint32 ZipHeader::offset() const
{
    return m_data->offset;
}

void ZipHeader::setCrc32(qint32 crc32)
{
    m_data->crc32 = crc32;
}

qint32 ZipHeader::crc32() const
{
    return m_data->crc32;
}

void ZipHeader::setCompressedSize(qint32 size)
{
    m_data->cSize = size;
}

qint32 ZipHeader::compressedSize() const
{
    return m_data->cSize;
}

void ZipHeader::setUncompressedSize(qint32 size)
{
    m_data->uSize = size;
}

qint32 ZipHeader::uncompressedSize() const
{
    return m_data->uSize;
}
