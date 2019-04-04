/*
    This program is free software: you can redistribute it and/or modify
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

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QString>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("compressor"));

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("source"), QStringLiteral("Source file."));
    parser.addPositionalArgument(QStringLiteral("destination"),
                                 QStringLiteral("Destination file."));
    QCommandLineOption decmpOpt(QStringList{QStringLiteral("d"), QStringLiteral("decompress")},
                                QStringLiteral("Decompress."));
    parser.addOption(decmpOpt);
    QCommandLineOption formatOpt(QStringList{QStringLiteral("f"), QStringLiteral("format")},
                                 QStringLiteral("Compression format."),
                                 QStringLiteral("format-value Zlib, Gzip, RawDeflate"),
                                 QStringLiteral("Zlib"));
    parser.addOption(formatOpt);
    QCommandLineOption lvlOpt(QStringList{QStringLiteral("l"), QStringLiteral("level")},
                                QStringLiteral("Compression level. Ignores if decompress."),
                                QStringLiteral("level-value 0-9"));
    parser.addOption(lvlOpt);
    parser.addHelpOption();
    parser.process(app);

    //Check file arguments.
    const QStringList args = parser.positionalArguments();
    if (args.size() < 2)
    {
        qDebug() << "Too few arguments!";
        return 1;
    }

    //Check format option if present.
    const QString frmtVal = parser.value(formatOpt);
    ZCompressor::CompressFormat frmt = ZCompressor::ZlibFormat;
    if (!frmtVal.compare(QStringLiteral("Zlib"), Qt::CaseInsensitive))
    { }
    else if (!frmtVal.compare(QStringLiteral("Gzip"), Qt::CaseInsensitive))
        frmt = ZCompressor::GzipFormat;
    else if (!frmtVal.compare(QStringLiteral("RawDeflate"), Qt::CaseInsensitive))
        frmt = ZCompressor::RawDeflateFormat;
    else
    {
        qDebug() << "Invalid compression format! Must be Zlib, Gzip or RawDeflate.";
        return 1;
    }

    //Check compression level if decompression option is not set.
    int lvl = -2;
    const bool decmp = parser.isSet(decmpOpt);
    if (!decmp)
    {
        const QString lvlVal = parser.value(lvlOpt);
        bool ok = false;
        lvl = lvlVal.toInt(&ok);
        if (!ok)
            lvl = Z_DEFAULT_COMPRESSION;
        else if (lvl < 0 || lvl > 9)
        {
            qDebug() << "Invalid compression level! Must bee between 0 and 9.";
            return 1;
        }
    }

    //Open files.
    QFile src(args.at(0));
    QFile dest(args.at(1));
    if (!src.open(QIODevice::ReadOnly) || !dest.open(QIODevice::WriteOnly))
    {
        //Files closes in destructor if necessary.
        qDebug() << "Can't open source or destination file!";
        return 1;
    }

    //Compress or decompress.
    int ret = 0;
    if (decmp)
        ret = ZCompressor::inf(&src, &dest, frmt);
    else
        ret = ZCompressor::def(&src, &dest, lvl, frmt);

    src.close();
    dest.close();

    //Success or failed compression.
    if (ret == 0)
    {
        qDebug() << "Success!";
        return 0;
    }
    else
    {
        qDebug() << "Failed!";
        return 1;
    }
}
