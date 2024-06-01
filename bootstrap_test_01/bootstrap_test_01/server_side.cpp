// server.cpp
#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <sstream>

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

void handle_client(tcp::socket& socket) {
    try {
        Data data{ "!", {1, 2, 3, 4, 5} };

        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << data;

        std::string outbound_data = archive_stream.str();
        std::ostringstream header_stream;
        header_stream << std::setw(8) << std::hex << outbound_data.size();

        std::string header = header_stream.str();
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(header));
        buffers.push_back(boost::asio::buffer(outbound_data));

        boost::asio::write(socket, buffers);
    }
    catch (std::exception& e) {
        std::cerr << "Exception in handle_client: " << e.what() << "\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            handle_client(socket);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
