#pragma once
// just a draft file. **it won't compile
// will be deleted at the production time
#if 0

// https://www.boost.org/doc/libs/1_87_0/doc/html/boost_asio/tutorial/tutdaytime2.html
std::string ipAddress = "127.0.0.1";
std::string portNum = "3000";

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/beast/http.hpp>
#include <boost/thread.hpp>
#include <array>
#include <iostream>
#include <boost/asio.hpp>

namespace http = boost::beast::http;
namespace beast = boost::beast;
using endPoint = tcp::endpoint;

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "daytime");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        for (;;)
        {
            std::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    console.log(BOOST_VERSION);

    return 0;
}



#endif