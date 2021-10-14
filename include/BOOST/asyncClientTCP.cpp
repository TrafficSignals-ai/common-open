

//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications Copyright (c) 2021 TrafficSignals-ai
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <list>

#include <thread>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class asyncClientTCP
{
private:

    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;

    std::string _messageEnd = "\r\n";

    std::list<std::string> _messageReadBuffer;
    std::mutex _messageReadBufferMutex;         //!< Mutex for the buffer

    void handle_resolve(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to the first endpoint in the list. Each endpoint
            // will be tried until we successfully establish a connection.
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                boost::bind(&asyncClientTCP::handle_connect, this,
                boost::asio::placeholders::error, ++endpoint_iterator));
        }
        else
        {
            std::cout << "asyncClientTCP::handle_resolve Error: " << err.message() << "\n";            
            std::this_thread::sleep_for (std::chrono::seconds(5));
        }
    }

  void handle_connect(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            std::cout << "Connected." << std::endl;
            boost::asio::async_read_until(socket_, response_, _messageEnd,
                boost::bind(&asyncClientTCP::handle_read, this,
                boost::asio::placeholders::error));

        }
        else if (endpoint_iterator != tcp::resolver::iterator())
        {
            // The connection failed. Try the next endpoint in the list.
            socket_.close();
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                boost::bind(&asyncClientTCP::handle_connect, this,
                boost::asio::placeholders::error, ++endpoint_iterator));
        }
        else
        {
            std::cout << "asyncClientTCP::handle_connect Error: " << err.message() << "\n";
        }
    }


    void handle_write(const boost::system::error_code& err)
    {
        if (err)
        {
            std::cout << "asyncClientTCP::handle_write Error: " << err.message() << "\n";
        }
    }

    void handle_read(const boost::system::error_code& err)
    {
        if (!err)
        {
            std::istream responseStream(&response_);
            std::string messageRead(std::istreambuf_iterator<char>(responseStream), {});

            std::unique_lock<std::mutex> pushGuard(_messageReadBufferMutex);
            _messageReadBuffer.push_back(messageRead);
            //int bufferSize = _messageReadBuffer.size();
            pushGuard.unlock();
            //std::cout << "Message received & buffered. Size: " << bufferSize << std::endl;

            boost::asio::async_read_until(socket_, response_, _messageEnd,
                boost::bind(&asyncClientTCP::handle_read, this,
                boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "asyncClientTCP::handle_read Error: " << err << "\n";
        }
    }


public:

    asyncClientTCP(boost::asio::io_service& io_service,
        const std::string& server, const std::string& port)
        : resolver_(io_service),
        socket_(io_service)
    {
        //std::cout << "Connecting to " << server << ":" << port << " and waiting." << std::endl;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(server, port);
        resolver_.async_resolve(query,
            boost::bind(&asyncClientTCP::handle_resolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
    }

    void SendMessage(std::string message)
    {
        //std::cout << "Message sending: " << message << std::endl;

        // The connection was successful. Send the request.
        boost::asio::async_write(socket_, boost::asio::buffer(message),
            boost::bind(&asyncClientTCP::handle_write, this,
            boost::asio::placeholders::error));
    }

    std::string ReceiveMessage()
    {
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

    int BufferSize()
    {   
        std::unique_lock<std::mutex> sizeGuard(_messageReadBufferMutex);
        int bufferSize = _messageReadBuffer.size();
        sizeGuard.unlock();
        return bufferSize;
    };

    void ClearBuffer()
    {
        std::unique_lock<std::mutex> clearGuard(_messageReadBufferMutex);
        _messageReadBuffer.clear();
        clearGuard.unlock();

    }
};