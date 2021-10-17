

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

#ifndef ASYNC_CLIENT_H
#define ASYNC_CLIENT_H

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

    std::vector<std::shared_ptr<std::string>> *_messageReadBuffer;
    std::mutex _mx;


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
        std::cout << "asyncClientTCPManager::handle_connect" << std::endl;

        if (!err)
        {
            std::cout << "asyncClientTCPManager::handle_connect Connected." << std::endl;
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
        std::cout << "asyncClientTCP::handle_read" << std::endl;
        if (!err)
        {
            std::istream responseStream(&response_);
            std::string messageRead(std::istreambuf_iterator<char>(responseStream), {});

            //std::shared_ptr<std::string> messagePtr = 

            //std::unique_lock<std::mutex> pushGuard(_mx);
            _messageReadBuffer->push_back(std::make_shared<std::string>(messageRead));
            
            int bufferSize = _messageReadBuffer->size();
            //pushGuard.unlock();
            std::cout << "Message received & buffered. Size: " << bufferSize << std::endl;

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
        std::cout << "asyncClientTCP::ReceiveMessage" << std::endl;

        std::string receivedMessage = "";

        while (receivedMessage.length() == 0)
        {   
            //std::unique_lock<std::mutex> sizeGuard(_mx);
            int bufferSize = _messageReadBuffer->size();
            //sizeGuard.unlock();

            if (bufferSize == 0)
            {
                std::this_thread::sleep_for (std::chrono::milliseconds(50));
            }
            else
            {
                //std::unique_lock<std::mutex> popGuard(_mx);
                std::shared_ptr<std::string> receivedMessagePtr = std::move(_messageReadBuffer->front());
                receivedMessage = *receivedMessagePtr;
                _messageReadBuffer->pop_back();
                //popGuard.unlock();

                bufferSize--;
                std::cout << "Message popped from buffer. Size: " << bufferSize << std::endl;
            }
        }

        return receivedMessage;
    };

    int BufferSize()
    {   
        //std::lock_guard<std::mutex> lk(_mx);
        int bufferSize = _messageReadBuffer->size();
        return bufferSize;
    };

    void ClearBuffer()
    {
        //std::lock_guard<std::mutex> lk(_mx);
        _messageReadBuffer->clear();
    }
};

class asyncClientTCPManager
{
private: 
    asyncClientTCP *_client;  //!< TODO: The current server class.  
    std::thread _threadStart; //!< Thread container for the Start method
    
    bool _healthy = false;  //!< Store if the server is healthy.  
    int _port;
    std::string _address;

public: 
    /*!
    \fn asyncServerTCPManager
    \brief A constructor for the connection manager class. 
    \return void
    */
    asyncClientTCPManager(std::string address, int port)
    {        
        std::cout << "asyncClientTCPManager::asyncClientTCPManager " << address << ":" << std::to_string(port) << std::endl;
        _port = port;
        _threadStart = std::thread(&asyncClientTCPManager::start, this);
        return;
    }

    /*!
    \fn ~asyncServerTCPManager
    \brief Clear up all pointers, lists and objects. 
    \return void
    */
    ~asyncClientTCPManager()
    {
        std::cout << "asyncClientTCPManager::~asyncClientTCPManager" << std::endl;
        _healthy = false;
    }

    /*!
    \fn Start
    \brief Starts the server listening.
    \return void
    */
    void start()
    {
        while(true)
        {
            try
            {
                std::cout << "asyncClientTCPManager::start" << std::endl;
                boost::asio::io_service io_service;
                asyncClientTCP asyncClient(io_service, _address, std::to_string(_port));
                _healthy = true;
                io_service.run(); //blocking
            }
            catch (std::exception& e)
            {
                std::cout << "asyncClientTCPManager::start::Exception: " << e.what() << "\n";
            }

            _healthy = false;
            std::cout << "asyncClientTCPManager::start CLIENT CONNECTION FAILED." << std::endl;
            std::this_thread::sleep_for (std::chrono::seconds(5)); // nothing to process
        }
    }

    /*!
    \fn SendMessage
    \brief Send a message to all connected clients. 
    \param message the text desired to be sent to all connected parties.
    \return void
    */
    void send(std::string message)
    {
        std::cout << "asyncClientTCPManager::send" << std::endl;
        message = message + "\r\n";
        if (healthy())
        {
            _client->SendMessage(message);
        }
        else
        {
            std::cout << "asyncClientTCPManager::send:ERROR: Client unhealthy, message not sent." << std::endl;
        }
    }

    /*!
    \fn receive
    \brief Receive a message from the client connection. 
    \warning This is a blocking call. 
    \return the text received.
    */
    std::string receive()
    {
        std::cout << "asyncClientTCPManager::receive" << std::endl;
        
        while (true)
        {
            if (healthy())
            {
                return _client->ReceiveMessage();
            }
            else
            {
                std::cout << "asyncClientTCPManager::receive Client unhealthy. Waiting." << std::endl;
                std::this_thread::sleep_for (std::chrono::seconds(5)); // nothing to process
            }

        }


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

    void clear_buffer()
    {
        if (healthy())
        {
            _client->ClearBuffer();
        }
    }

};
#endif