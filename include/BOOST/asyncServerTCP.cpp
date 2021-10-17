//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H


#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <list>

using boost::asio::ip::tcp;

class asyncConnectionTCP : public boost::enable_shared_from_this<asyncConnectionTCP>
{
public:
    typedef boost::shared_ptr<asyncConnectionTCP> pointer;

    static pointer create(boost::asio::io_context& io_context)
    {
        std::cout << "asyncConnectionTCP::create" << std::endl;
        return pointer(new asyncConnectionTCP(io_context));

    }

    tcp::socket& socket()
    {
        std::cout << "asyncConnectionTCP::socket" << std::endl;
        return socket_;
    }

    void start()
    {
        std::cout << "asyncConnectionTCP::start" << std::endl;
        boost::asio::async_read_until(socket_, response_, _messageEnd,
            boost::bind(&asyncConnectionTCP::handle_read, this,
            boost::asio::placeholders::error));
    }

    void send_message_async(std::string message)
    {
        std::cout << "asyncConnectionTCP::send_message_async: " << message << std::endl;
        boost::asio::async_write(socket_, boost::asio::buffer(message),
            boost::bind(&asyncConnectionTCP::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    int buffer_size()
    {   
        std::cout << "asyncConnectionTCP::buffer_size" << std::endl;
        std::unique_lock<std::mutex> sizeGuard(_messageReadBufferMutex);
        int bufferSize = _messageReadBuffer.size();
        sizeGuard.unlock();
        return bufferSize;
    };

    void clear_buffer()
    {
        std::cout << "asyncConnectionTCP::clear_buffer" << std::endl;
        std::unique_lock<std::mutex> clearGuard(_messageReadBufferMutex);
        _messageReadBuffer.clear();
        clearGuard.unlock();

    }

    std::string receive_message()
    {
        std::cout << "asyncConnectionTCP::receive_message" << std::endl;
        std::string receivedMessage = "";

        while (receivedMessage.length() == 0)
        {
            std::unique_lock<std::mutex> sizeGuard(_messageReadBufferMutex);
            int bufferSize = _messageReadBuffer.size();
            sizeGuard.unlock();

            if (bufferSize == 0)
            {
                std::this_thread::sleep_for (std::chrono::milliseconds(50));
            }
            else
            {
                std::unique_lock<std::mutex> popGuard(_messageReadBufferMutex);
                receivedMessage = _messageReadBuffer.front();
                _messageReadBuffer.pop_front();
                popGuard.unlock();

                bufferSize--;
                std::cout << "Message popped from buffer. Size: " << bufferSize << std::endl;
            }
        }

        return receivedMessage;
    };


private:
    asyncConnectionTCP(boost::asio::io_context& io_context) : socket_(io_context)
    {
        std::cout << "asyncConnectionTCP::asyncConnectionTCP" << std::endl;

    }

    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
    {
        std::cout << "asyncConnectionTCP::handle_write" << std::endl;
    }

    void handle_read(const boost::system::error_code& err)
    {
        std::cout << "asyncConnectionTCP::handle_read" << std::endl;
        if (!err)
        {
            std::istream responseStream(&response_);
            std::string messageRead(std::istreambuf_iterator<char>(responseStream), {});

            std::unique_lock<std::mutex> pushGuard(_messageReadBufferMutex);
            _messageReadBuffer.push_back(messageRead);
            int bufferSize = _messageReadBuffer.size();
            pushGuard.unlock();
            std::cout << "Message received & buffered. Size: " << bufferSize << std::endl;

            boost::asio::async_read_until(socket_, response_, _messageEnd,
                boost::bind(&asyncConnectionTCP::handle_read, this,
                boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "asyncConnectionTCP::handle_read Error: " << err << "\n";
        }
    }
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    tcp::socket socket_;
    std::string _messageEnd = "\r\n";

    std::list<std::string> _messageReadBuffer;
    std::mutex _messageReadBufferMutex;         //!< Mutex for the buffer
};




class asyncServerTCP
{

private:

    using connptr = boost::shared_ptr<asyncConnectionTCP>;
    using weakptr = boost::weak_ptr<asyncConnectionTCP>;

    std::mutex _mx;
    std::vector<weakptr> _registered;

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;


    void reg_connection(weakptr wp) 
    {
        std::cout << "asyncServerTCP::reg_connection" << std::endl;
        std::lock_guard<std::mutex> lk(_mx);
        _registered.push_back(wp);
        return;
    }

    void start_accept()
    {
        std::cout << "asyncServerTCP::start_accept" << std::endl;
        asyncConnectionTCP::pointer new_connection = asyncConnectionTCP::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&asyncServerTCP::handle_accept, this, new_connection,
            boost::asio::placeholders::error));
    }

    void handle_accept(asyncConnectionTCP::pointer new_connection, const boost::system::error_code& error)
    {
        std::cout << "asyncServerTCP::handle_accept" << std::endl;
        if (!error)
        {
            auto weak = boost::weak_ptr<asyncConnectionTCP>(new_connection);

            reg_connection(weak);
            new_connection->start();
        }

        start_accept();
    }


public:


    asyncServerTCP(boost::asio::io_context& io_context, int port) : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "asyncServerTCP::asyncServerTCP" << std::endl;
        start_accept();
    }

    void send_all_async(std::string message)
    {
        std::cout << "asyncServerTCP::get_next_buffered_message" << std::endl;
        std::string receivedMessage = "";
        
        std::vector<connptr> active;
        {
            std::lock_guard<std::mutex> lk(_mx);
            for (auto& w : _registered)
            {
                if (auto c = w.lock())
                {
                    active.push_back(c);
                }
            }
        }

        for (auto& connection : active) 
        {
            connection->send_message_async(message);
        }
        return;
    }



    std::string get_next_buffered_message()
    {
        std::cout << "asyncServerTCP::get_next_buffered_message" << std::endl;
        std::string receivedMessage = "";

        while (receivedMessage.length() == 0)
        {
            std::vector<connptr> active;
            {
                std::lock_guard<std::mutex> lk(_mx);
                for (auto& w : _registered)
                {
                    if (auto c = w.lock())
                    {
                        active.push_back(c);
                    }
                }
            }

            for (auto& connection : active) 
            {
                int connectionBufferSize = connection->buffer_size();

                std::cout << "asyncServerTCP::get_next_buffered_message: Buffer Size: " << connectionBufferSize << std::endl;

                if (connectionBufferSize > 0)
                {
                    receivedMessage = connection->receive_message();
                    std::cout << "asyncServerTCP::get_next_buffered_message: Received message: " << receivedMessage << std::endl;
                }

            }

            if (receivedMessage.length() == 0)
            {
                std::this_thread::sleep_for (std::chrono::milliseconds(50));
            }
        }

        return receivedMessage;
    }


};



/*!
    \class asyncServerTCPManager
    \brief Represents a management service for the server

    Responsability
    --------------
    Control the server and ensure it is healthy

    Collaboration
    -------------
    Used by objects detecting where an object has been detected within a 2d plane. 
    \sa ServerTCP()
*/
class asyncServerTCPManager
{
private: 
    asyncServerTCP *_server;  //!< TODO: The current server class.  
    std::thread _threadStart; //!< Thread container for the Start method
    
    bool _healthy;  //!< Store if the server is healthy.  
    int _port;

public: 
    /*!
    \fn asyncServerTCPManager
    \brief A constructor for the connection manager class. 
    \return void
    */
    asyncServerTCPManager(int port)
    {        
        std::cout << "asyncServerTCPManager::asyncServerTCPManager" << std::endl;

        _healthy = true;
        _port = port;
        std::cout << "main::createServer initialised." << std::endl;
        _threadStart = std::thread(&asyncServerTCPManager::start, this);
        return;
    }

    /*!
    \fn ~asyncServerTCPManager
    \brief Clear up all pointers, lists and objects. 
    \return void
    */
    ~asyncServerTCPManager()
    {
        std::cout << "asyncServerTCPManager::~asyncServerTCPManager" << std::endl;
        //_threadStart.~thread();
        //_server->~asyncServerTCP();
        _healthy = false;
    }

    /*!
    \fn Start
    \brief Starts the server listening.
    \return void
    */
    void start()
    {
        std::cout << "asyncServerTCPManager::start" << std::endl;
        boost::asio::io_context context;
        asyncServerTCP server(context, _port);
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
    void send_all(std::string message)
    {
        std::cout << "asyncServerTCPManager::send_all" << std::endl;
        _server->send_all_async(message);
    }

    /*!
    \fn receive
    \brief Receive a message to all connected clients. 
    \return the text received.
    */
    std::string receive()
    {
        std::cout << "asyncServerTCPManager::receive" << std::endl;
        return _server->get_next_buffered_message();
    }


    /*!
    \fn Healthy
    \brief Returns if the connection manager is healthy. 
    \return bool
    */
    bool healthy()
    {
        return _healthy;
    }


};

#endif