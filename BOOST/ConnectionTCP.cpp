//          Copyright TrafficSignals.ai 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file BOOST_LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <stdio.h>
#include <iostream>
#include <memory>

#include <string>

#include <chrono>
#include <ctime>

#include <thread>
#include <mutex>

#include <list>

#include <stdexcept>

#include <utility>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

/*!
    \class ConnectionTCP
    \brief Represents a connection made over TCP

    Responsability
    --------------
    Control connections by clients. 

    Collaboration
    -------------
    Used by objects detecting where an object has been detected within a 2d plane. 
    \sa ServerTCP()
*/
class ConnectionTCP : public boost::enable_shared_from_this<ConnectionTCP>
{
public:
    typedef boost::shared_ptr<ConnectionTCP> pointer;

    /*!
    \fn ConnectionTCP
    \brief Instantiate the object, particularly the parent.  
    \param io_context the server context in which to create the connection.
    \return void
    */
    ConnectionTCP(boost::asio::io_context& io_context) : socket_(io_context)
    {
        std::cout << "ConnectionTCP::ConnectionTCP initalised." << std::endl;
        return;
    }

    /*!
    \fn create
    \brief Create a new ConnectionTCP pointer
    \param io_context the server context in which to create the connection.
    \return pointer
    */
    static pointer create(boost::asio::io_context& io_context)
    {
        std::cout << "ConnectionTCP::create Creating new ConnectionTCP from io_context." << std::endl;
        return pointer(new ConnectionTCP(io_context));
    }

    /*!
    \fn socket
    \brief Get an instance of the socket
    \return &socket
    */
    tcp::socket& socket()
    {
        //std::cout << "ConnectionTCP::socket Returning socket_." << std::endl;
        return socket_;
    }

    /*!
    \fn SendMessage
    \brief Send a message to the connected client. 
    \param message the text desired to be sent to all connected parties.
    \return void
    */
    void SendMessage(std::string message)
    {

        boost::asio::async_write(socket_, boost::asio::buffer(message),
            boost::bind(&ConnectionTCP::HandleWrite, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        //std::cout << "ConnectionTCP::start Sent message: " << message;

    }


private:

    /*!
    \fn HandleWrite
    \brief The handler method for when a message has been written.
    \param error_code the error structure returned as a consiquence of writing the message
    \param bytes_transferred the number of bytes sent.
    \return void
    */
    void HandleWrite(const boost::system::error_code& error_code /*error*/, size_t bytes_transferred /*bytes_transferred*/)
    {
        
        if (error_code.failed())
        {
            if (error_code.value() == 32)
            {
                socket_.close();
                std::cout << "ConnectionTCP::handle_write Connection closed: " << error_code.value() << "::" << error_code.message() << std::endl;
            }
            else if (error_code.value() == 9)
            {
                socket_.close();
                std::cout << "ConnectionTCP::handle_write Connection awaiting removal: " << error_code.value() << "::" << error_code.message() << std::endl;
            }
            else
            {
                socket_.close();
                std::cout << "ConnectionTCP::handle_write ERROR: Unhandled socket error, closed connection: " << error_code.value() << "::" << error_code.message() << std::endl;
            }
        }
        return;
    }

    tcp::socket socket_; //!< The active socket used with the client. 
};




/*!
    \class ServerTCP
    \brief Represents a server ready to receive TCP connections

    Responsability
    --------------
    Control connections by clients. 

    Collaboration
    -------------
    Used by a connection manager to manage connections.
    
    \sa ConnectionTCP()
*/
class ServerTCP
{
public:

    /*!
    \fn ServerTCP
    \brief A constructor for the server. 
    \param io_context the server context in which to create the connection.
    \return void
    */
    ServerTCP(boost::asio::io_context& io_context, int port) : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        _threadMaintainConnections = std::thread(&ServerTCP::MaintainConnections, this);
        _port = port;
        std::cout << "ServerTCP::ServerTCP Created. Port: " << _port << std::endl;

        CreateAcceptHandler();
    }

    /*!
    \fn ~ServerTCP
    \brief A destructor for the server. 
    \return void
    */
    ~ServerTCP()
    {
        io_context_.reset();
        acceptor_.release();

        while (_connections.size() != 0)
        {
            ConnectionTCP::pointer _connection = _connections.front();
            _connection.reset();
            _connections.pop_front();
        }
        
        _threadMaintainConnections.~thread();

        std::cout << "ServerTCP::~ServerTCP Completed." << _port << std::endl;
    }

    /*!
    \fn SendMessage
    \brief Send a message to all connected clients. 
    \param message the text desired to be sent to all connected parties.
    \return void
    */
    void SendMessage(std::string message)
    {
        std::unique_lock<std::mutex> iterateGuard(_connectionsMutex);
       
        for (ConnectionTCP::pointer & connection : _connections)
        {
            connection->SendMessage(message);
        }

        //if (_connections.size() > 0)
        //{
        //    std::cout << "ServerTCP::sendMessage Sent count: " << _connections.size() << std::endl;
        //}
        //else
        //{
        //    std::cout << "ServerTCP::sendMessage no active connections." << std::endl;
        //}

        iterateGuard.unlock();

        return;
    }

private:
    
    /*!
    \fn CreateAcceptHandler
    \brief Create a handler which can receive new connections.
    \return void
    */
    void CreateAcceptHandler()
    {
        ConnectionTCP::pointer new_connection = ConnectionTCP::create(io_context_);

        // Create new conneciton handler
        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&ServerTCP::HandleConnection, this, new_connection,
            boost::asio::placeholders::error));
    }

    /*!
    \fn HandleConnection
    \brief Manages the new connection request from the client. 
    \param new_connection ConnectionTCP made by the client. 
    \param error an error structure. 
    \return void
    */
    void HandleConnection(ConnectionTCP::pointer new_connection, const boost::system::error_code& error)
    {
        if (!error)
        {
            //new_connection->SendMessage("====================================\n");
            //new_connection->SendMessage("Connected to \"" + _serverName + "\"\n");
            //new_connection->SendMessage("====================================\n");
            _connections.push_back(new_connection);
        }

        CreateAcceptHandler();
    }

    /*!
    \fn MaintainConnections
    \brief Manages the list of current connections. 
    \return void
    */
    void MaintainConnections()
    {
        std::size_t lastConnectionCount = 0;

        while(true)
        {
            std::unique_lock<std::mutex> iterateGuard(_connectionsMutex);

            for (std::size_t i = 0; i < _connections.size(); i++)
            {
                auto it = std::next(_connections.begin(), i);

                ConnectionTCP::pointer& connection = *it;
                tcp::socket& socket = connection->socket();

                if (!socket.is_open())
                {
                    std::cout << "ServerTCP::sendMessages Connecton closed, removing index: " << i << std::endl;
                    _connections.remove(*it);
                }
            }

            if (lastConnectionCount != _connections.size())
            {
                std::cout << "ServerTCP::MaintainConnections Maintaining connections: " << _connections.size() << std::endl;
                lastConnectionCount = _connections.size();
            }

            iterateGuard.unlock();

            std::this_thread::sleep_for (std::chrono::seconds(1));
        }
        return;
    }

    boost::asio::io_context& io_context_; //!< The contect of the connection
    tcp::acceptor acceptor_; //!< The acceptor class used

    std::list<ConnectionTCP::pointer> _connections; //!< List of current connections
    std::mutex _connectionsMutex; //!< Mutex for the connections list
    
    std::thread _threadMaintainConnections; //!< Thread container for the MaiantainConnections method

    int _port; //!< TODO: The port number ot listen on.
    std::string _serverName = "Boost.ASIO Test"; //!< TODO: Textual description of the server. 

};

/*!
    \class ConnectionManager
    \brief Represents a management service for the server

    Responsability
    --------------
    Control the server and ensure it is healthy

    Collaboration
    -------------
    Used by objects detecting where an object has been detected within a 2d plane. 
    \sa ServerTCP()
*/
class ConnectionManager
{
private: 
    ServerTCP *_server;  //!< TODO: The current server class.  
    std::thread _threadStart; //!< Thread container for the Start method
    
    bool _healthy;  //!< Store if the server is healthy.  
    int _port;

public: 
    /*!
    \fn ConnectionManager
    \brief A constructor for the connection manager class. 
    \return void
    */
    ConnectionManager(int port)
    {
        _healthy = true;
        _port = port;
        std::cout << "main::createServer initalised." << std::endl;
        _threadStart = std::thread(&ConnectionManager::Start, this);
        return;
    }

    /*!
    \fn ~ConnectionManager
    \brief Clear up all pointers, lists and objects. 
    \return void
    */
    ~ConnectionManager()
    {
        _threadStart.~thread();
        _server->~ServerTCP();
        _healthy = false;
    }

    /*!
    \fn Start
    \brief Starts the server listening.
    \return void
    */
    void Start()
    {
        boost::asio::io_context context;
        ServerTCP server(context, _port);
        _server = &(server);
        _healthy = true;
        context.run();

        //This will only run if the server fails....
        _healthy = false;
        std::cout << "main::createServer SERVER FAILED." << std::endl;
    }

    /*!
    \fn SendMessage
    \brief Send a message to all connected clients. 
    \param message the text desired to be sent to all connected parties.
    \return void
    */
    void SendMessage(std::string message)
    {
        _server->SendMessage(message);
    }

    /*!
    \fn Healthy
    \brief Returns if the connection manager is healthy. 
    \return bool
    */
    bool Healthy()
    {
        return _healthy;
    }


};


/*
    TCP Error Reference

    0 = Success
    1 = Operation not permitted
    2 = No such file or directory
    3 = No such process
    4 = Interrupted system call
    5 = Input/output error
    6 = No such device or address
    7 = Argument list too long
    8 = Exec format error
    9 = Bad file descriptor
    10 = No child processes
    11 = Resource temporarily unavailable
    12 = Cannot allocate memory
    13 = Permission denied
    14 = Bad address
    15 = Block device required
    16 = Device or resource busy
    17 = File exists
    18 = Invalid cross-device link
    19 = No such device
    20 = Not a directory
    21 = Is a directory
    22 = Invalid argument
    23 = Too many open files in system
    24 = Too many open files
    25 = Inappropriate ioctl for device
    26 = Text file busy
    27 = File too large
    28 = No space left on device
    29 = Illegal seek
    30 = Read-only file system
    31 = Too many links
    32 = Broken pipe
    33 = Numerical argument out of domain
    34 = Numerical result out of range
    35 = Resource deadlock avoided
    36 = File name too long
    37 = No locks available
    38 = Function not implemented
    39 = Directory not empty
    40 = Too many levels of symbolic links
    41 = Unknown error 41
    42 = No message of desired type
    43 = Identifier removed
    44 = Channel number out of range
    45 = Level 2 not synchronized
    46 = Level 3 halted
    47 = Level 3 reset
    48 = Link number out of range
    49 = Protocol driver not attached
    50 = No CSI structure available
    51 = Level 2 halted
    52 = Invalid exchange
    53 = Invalid request descriptor
    54 = Exchange full
    55 = No anode
    56 = Invalid request code
    57 = Invalid slot
    58 = Unknown error 58
    59 = Bad font file format
    60 = Device not a stream
    61 = No data available
    62 = Timer expired
    63 = Out of streams resources
    64 = Machine is not on the network
    65 = Package not installed
    66 = Object is remote
    67 = Link has been severed
    68 = Advertise error
    69 = Srmount error
    70 = Communication error on send
    71 = Protocol error
    72 = Multihop attempted
    73 = RFS specific error
    74 = Bad message
    75 = Value too large for defined data type
    76 = Name not unique on network
    77 = File descriptor in bad state
    78 = Remote address changed
    79 = Can not access a needed shared library
    80 = Accessing a corrupted shared library
    81 = .lib section in a.out corrupted
    82 = Attempting to link in too many shared libraries
    83 = Cannot exec a shared library directly
    84 = Invalid or incomplete multibyte or wide character
    85 = Interrupted system call should be restarted
    86 = Streams pipe error
    87 = Too many users
    88 = Socket operation on non-socket
    89 = Destination address required
    90 = Message too long
    91 = Protocol wrong type for socket
    92 = Protocol not available
    93 = Protocol not supported
    94 = Socket type not supported
    95 = Operation not supported
    96 = Protocol family not supported
    97 = Address family not supported by protocol
    98 = Address already in use
    99 = Cannot assign requested address
    100 = Network is down
    101 = Network is unreachable
    102 = Network dropped connection on reset
    103 = Software caused connection abort
    104 = Connection reset by peer
    105 = No buffer space available
    106 = Transport endpoint is already connected
    107 = Transport endpoint is not connected
    108 = Cannot send after transport endpoint shutdown
    109 = Too many references: cannot splice
    110 = Connection timed out
    111 = Connection refused
    112 = Host is down
    113 = No route to host
    114 = Operation already in progress
    115 = Operation now in progress
    116 = Stale NFS file handle
    117 = Structure needs cleaning
    118 = Not a XENIX named type file
    119 = No XENIX semaphores available
    120 = Is a named type file
    121 = Remote I/O error
    122 = Disk quota exceeded
    123 = No medium found
    124 = Wrong medium type
*/
