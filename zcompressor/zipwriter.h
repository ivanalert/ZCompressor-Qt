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
