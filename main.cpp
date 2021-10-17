/*
 *                    Copyright 2021 TrafficSignals.ai
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following 
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information on this licence, please refer to: 
 * https://opensource.org/licenses/MIT
 * 
 */


#include <memory>

#include <cctype>

#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <ctime>
#include <chrono>         // std::chrono::seconds

#include <iomanip>
#include <fstream>
#include <iostream>

#include <string>

#include <map>

#include <sys/stat.h>

#include <string>

#include "include/BOOST/ConnectionTCP.cpp"
#include "include/BOOST/asyncClientTCP.cpp"
#include "include/BOOST/asyncServerTCP.cpp"


void BoostServerExample()
{
    const int port = 8000;
    std::unique_ptr connectionManagerPtr = std::unique_ptr<ConnectionManager>(new ConnectionManager(port));

    std::this_thread::sleep_for (std::chrono::seconds());
   
    std::cout << "main Executing." << std::endl;
    try
    {
        int i = 0;

        while(true)
        {
            if (connectionManagerPtr->Healthy())
            {
                connectionManagerPtr->SendMessage(std::to_string(i));
            }
            else
            {
                connectionManagerPtr->~ConnectionManager();
                std::this_thread::sleep_for (std::chrono::seconds(5));
                connectionManagerPtr = std::unique_ptr<ConnectionManager>(new ConnectionManager(port));
                std::this_thread::sleep_for (std::chrono::seconds(5));
            }
            i++;
            std::this_thread::sleep_for (std::chrono::milliseconds(250));
        }

        std::cout << "main run complete." << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

};

void BoostClientExample()
{
    const int port = 8000;
    const std::string tag = "Vision";

    ConnectionClient client("192.168.1.178", port);
    std::this_thread::sleep_for (std::chrono::seconds(1)); // nothing to process

    while (true)
    {
        std::string content = client.AwaitTag(tag);
        std::cout << "Tag received of " << content.size() << " characters. Buffer size: " << client.BufferSize() << std::endl;


    }
};

int asyncClientTCPManagerExample(int argc, char* argv[])
{
    std::cout << "Running asycClientTCPExample." << std::endl;    

    try
    {
        if (argc != 4)
        {
            std::cout << "Received: " << argv[0] << " " << argv[1] << "  " << argv[2] << "  " << argv[3] << std::endl;
            std::cout << "Usage: console asyncClientTCPManager <server> <port>" << std::endl;
            std::cout << "Example:" << std::endl;
            std::cout << "  console asyncClientTCPManager localhost 8000" << std::endl;
            return 1;
        }

        asyncClientTCPManager manager(argv[2], std::stoi(argv[3]));
        std::cout << "asyncClientTCPManager Created. " << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(5));

        while (true)
        {
            std::cout << "asyncClientTCPManager Waiting for message.... " << std::endl;
            std::string messageReceived = manager.receive();
            std::cout << "asyncClientTCPManager Received: " << messageReceived << std::endl;
            manager.send(messageReceived);
            std::cout << "asyncClientTCPManager Returned echo to all: " << std::endl << messageReceived << std::endl;
        }

        std::cout << "Failed to start." << std::endl;    

    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}


int asycServerTCPManagerExample(int argc, char* argv[])
{
    std::cout << "Running asycServerTCPExample." << std::endl;    

    try
    {
        if (argc != 3)
        {
            std::cout << "Received: " << argv[0] << " " << argv[1] << "  " << argv[2] << std::endl;
            std::cout << "Usage: console asycServerTCP <port>" << std::endl;
            std::cout << "Example:" << std::endl;
            std::cout << "  console asycClientTCP 8000" << std::endl;
            return 1;
        }

        asyncServerTCPManager manager(std::stoi(argv[2]));
        std::cout << "asyncServerTCPManager Created. " << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(5));

        while (true)
        {
            std::cout << "asyncServerTCPManager Waiting for message.... " << std::endl;
            std::string messageReceived = manager.receive();
            std::cout << "asyncServerTCPManager Received: " << messageReceived << std::endl;
            manager.send_all(messageReceived);
            std::cout << "asyncServerTCPManager Returned echo to all: " << std::endl << messageReceived << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}


int asycServerTCP_read()
{
    while(true)
    {
        
    }
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        if (!strcmp(argv[1], "asyncClientTCPManager"))        {
            return asyncClientTCPManagerExample(argc, argv);
        }
        else if (!strcmp(argv[1], "asycServerTCP"))        {
            return asycServerTCPManagerExample(argc, argv);
        }
    }
    else if (argc == 1)
    {
        char *array[4] = { "console", "asyncClientTCPManager", "localhost", "8000" };     
        return asyncClientTCPManagerExample(4, array);


        //return asycServerTCPExample(3, array);
    }

    //BoostServerExample();
    //BoostClientExample();
    //JetsonGPIOExample();
   
};
