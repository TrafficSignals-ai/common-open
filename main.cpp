//#include "include/ConnectionTCP.hpp"
#include "BOOST/ConnectionTCP.cpp"

#include <memory>

#include <cctype>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <chrono>         // std::chrono::seconds
#include <iomanip>

#include <string>

int main()
{
    std::unique_ptr connectionManagerPtr = std::unique_ptr<ConnectionManager>(new ConnectionManager());

    std::this_thread::sleep_for (std::chrono::seconds(5));
   
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
                connectionManagerPtr = std::unique_ptr<ConnectionManager>(new ConnectionManager());
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

    return 0;
}
