// client.cpp
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

using boost::asio::ip::tcp;

struct Data {
    std::string message;
    std::vector<int> numbers;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& message;
        ar& numbers;
    }
};

void receive_data(tcp::socket& socket) {
    try {
        char header[8];
        boost::asio::read(socket, boost::asio::buffer(header));
        std::istringstream header_stream(std::string(header, 8));
        std::size_t inbound_data_size = 0;
        header_stream >> std::hex >> inbound_data_size;

        std::vector<char> inbound_data(inbound_data_size);
        boost::asio::read(socket, boost::asio::buffer(inbound_data));

        std::string archive_data(&inbound_data[0], inbound_data.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);

        Data data;
        archive >> data;

        std::cout << "Message: " << data.message << "\n";
        std::cout << "Numbers: ";
        for (int number : data.numbers) {
            std::cout << number << " ";
        }
        std::cout << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception in receive_data: " << e.what() << "\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        receive_data(socket);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
