add_executable(amqp_listen amqp_listen.cpp)

target_link_libraries(amqp_listen
        PRIVATE
        SimpleAmqpClient)

add_executable(amqp_listenq amqp_listenq.cpp)

target_link_libraries(amqp_listenq
        PRIVATE
        SimpleAmqpClient)

add_executable(amqp_producer amqp_producer.cpp)

target_link_libraries(amqp_producer
        PRIVATE
        SimpleAmqpClient)