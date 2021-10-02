//
// Created by gennaro on 01/10/21.
//

#pragma once

#include <cmath>
#include <iomanip>
#include <vector>

namespace utils {

inline constexpr std::size_t line_length = 16;
inline constexpr std::size_t offset_span = 4;

template <typename S>
inline void print_offset(std::size_t start_offset, std::size_t end_offset,
                         S& stream) {
  stream << std::setw(offset_span) << start_offset << ':'
         << std::setw(offset_span) << end_offset << ' ';
}

template <typename S>
inline void print_line(const std::uint8_t* buffer, std::size_t size,
                       std::size_t line, S& stream) {
  const auto begin = line * line_length;
  const auto end = begin + std::min(line_length - 1, size - begin - 1);

  auto printable_buffer = std::string(line_length, ' ');

  print_offset(begin, end, stream);

  for (std::size_t i = begin, j = 0; i <= end; ++i, ++j) {
    const auto byte = buffer[i];

    stream << std::setw(2) << static_cast<int>(byte) << ' ';

    printable_buffer[j] = std::isprint(byte) ? printable_buffer[j] = byte : '.';
  }

  for (std::size_t i = end + 1; i < begin + line_length; ++i) {
    stream << "   ";
  }
  stream << printable_buffer << '\n';
}

template <typename S>
inline void dump(const void* data, std::size_t size, S& stream) {
  const auto lines = static_cast<std::size_t>(
      std::ceil(static_cast<double>(size) / line_length));

  auto buffer = reinterpret_cast<const std::uint8_t*>(data);

  auto old_set = stream.setf(std::ios::hex, std::ios::basefield);
  auto old_fill = stream.fill('0');

  for (std::size_t i = 0; i < lines; ++i) {
    print_line(buffer, size, i, stream);
  }

  stream << '\n';

  stream.setf(old_set);
  stream.fill(old_fill);
}

template <typename S>
inline void print_envelop(const AmqpClient::Envelope::ptr_t& envelope,
                          S& stream, const bool dump_message = true) {
  stream << "Delivery " << envelope->DeliveryTag()      //
         << ", exchange " << envelope->Exchange()       //
         << ", routing key " << envelope->RoutingKey()  //
         << '\n';

  if (envelope->Message()->ContentTypeIsSet()) {
    stream << "Content-type: " << envelope->Message()->ContentType() << '\n';
  }
  stream << "----\n";

  if (dump_message) {
    dump(envelope->Message()->Body().data(), envelope->Message()->Body().size(),
         std::cout);
  }
}

}  // namespace utils

inline std::ostream& operator<<(std::ostream& os,
                                const AmqpClient::AmqpException& ex) {
  os << "AmqpException:"                                            //
     << " is soft error: " << std::boolalpha << ex.is_soft_error()  //
     << ", reply code: " << ex.reply_code()                         //
     << ", reply text: " << ex.reply_text() << '\n';
  return os;
}
