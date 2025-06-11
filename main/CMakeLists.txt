idf_component_register(SRCS 
        "main.cpp"
        "interfaces/i2c-driver.cpp"
        "interfaces/i2c-register-interface.cpp"
        "networking/connection.cpp"
        "networking/packet-builder.cpp"
        "networking/packet-container.cpp"
        "networking/packet-parser.cpp"
        "networking/wifi-connection.cpp"
        "sensors/sensor-reader.cpp"
        "sensors/sensor-driver.cpp"
        INCLUDE_DIRS ".")
