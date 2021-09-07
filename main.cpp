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

#include "include/BOOST/ConnectionTCP.cpp"

#include <memory>

#include <cctype>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <chrono>         // std::chrono::seconds
#include <iomanip>

#include <string>

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

}

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
}

int main()
{
    //BoostServerExample();
    BoostClientExample();

    return 0;
}


