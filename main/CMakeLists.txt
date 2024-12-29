idf_component_register(SRCS 
                            "main.cpp"
                            "SmlCrc.cpp"
                            "SmlLexer.cpp"
                            "SmlParser.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES 
                              MqttClient
                              Wifi
                              driver
)
