/*
 * Copyright ( C ) 2007 Arnold Krille <arnold@arnoldarts.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "qoscclient.h"
//#include "qoscclient.moc"

//#include <QtCore/QDebug>
#include <QtCore/QByteArray>
#include <QtNetwork/QUdpSocket>

//#include <lo/lo.h>

QOscClient::QOscClient( const QHostAddress& address, quint16 port)
        : QOscBase()
        , _address( address )
        , _port( port )
{
//        qDebug() << "QOscClient::QOscClient(" << address << "," << port << "," << p << ")";
}

QOscClient::QOscClient( const QHostAddress& address, quint16 port, QObject* p )
        : QOscBase( p )
        , _address( address )
        , _port( port )
{
//        qDebug() << "QOscClient::QOscClient(" << address << "," << port << "," << p << ")";
}

QOscClient::QOscClient( const QHostAddress& address, quint16 source_port, quint16 dst_port, QObject* p )
        : QOscBase( p )
        , _address( address )
        , _port( dst_port )
{
    //    qDebug() << "QOscClient::QOscClient(" << address << "," << source_port << "," << dst_port << "," << p << ")";
    //    qDebug() << "- connectToHost(" << address << "," << dst_port << ")";
    //    socket()->bind( source_port , QUdpSocket::ShareAddress);
    //    socket()->bind( QHostAddress("192.168.1.81"),source_port );

    /*
     *  Bind to set source port
     */
    //socket()->connectToHost(address,dst_port,QIODevice::WriteOnly);
//    socket()->open(QIODevice::WriteOnly);
//    socket()->bind( source_port );
    socket()->bind( source_port, QUdpSocket::ShareAddress );
    // qDebug() << "- bind( 192.168.1.81," << source_port << ")";
}


QOscClient::~QOscClient() {
//        qDebug() << "QOscClient::~QOscClient()";
}

void QOscClient::setAddress( const QHostAddress& address, quint16 port ) {
	_address = address;
	_port = port;
}

void QOscClient::sendData( QString path, QVariant data ) {
//        qDebug() << "QOscClient::sendData(" << path << "," << data << ")";

        quint64 timeTag=getTimeTag();
        QByteArray msg = oscMessage( path, data );
        qint32 len = msg.length();

        QByteArray out(QOscBase::fromString("#bundle")+QOscBase::fromInt64(timeTag)+QOscBase::fromInt32(len)+msg);

        socket()->writeDatagram( out, _address, _port );

//        qDebug() << "to " << _address << " " << _port;
}
