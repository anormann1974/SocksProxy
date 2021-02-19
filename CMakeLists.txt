cmake_minimum_required(VERSION 3.10)
project(SocksProxy VERSION 1.0.0 LANGUAGES CXX C)

message(${CMAKE_VERSION})

#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Network LinguistTools REQUIRED)

if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
    get_target_property(_qt5_qmake_location Qt5::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt5_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    endif()
endif()

set(LIBRARY_HEADERS
    src/SocksSessionManager.h
    src/SocksSession.h
    src/SocksServer.h
    src/SocksConnection.h
    src/decorators/ThrottlingDecorator.h
    src/decorators/QIODeviceDecorator.h
    src/protocol/SocksReplyMessage4a.h
    src/protocol/SocksProtocolMessage.h
    src/protocol/Socks5UDPRequestMessage.h
    src/protocol/Socks5RequestMessage.h
    src/protocol/Socks5ReplyMessage.h
    src/protocol/Socks5MethodSelectionMessage.h
    src/protocol/Socks5GreetingMessage.h
    src/protocol/Socks4RequestMessage.h
    src/protocol/Socks4ReplyMessage.h
    src/states/SocksState.h
    src/states/Socks5UDPAssociateState.h
    src/states/Socks5UDPAssociatedState.h
    src/states/Socks5ConnectState.h
    src/states/Socks5ConnectedState.h
    src/states/Socks5AuthState.h
    src/states/Socks5AuthenticatedState.h
    src/states/Socks4InitialState.h
    src/states/Socks4ConnectState.h
    src/states/Socks4ConnectedState.h
    src/states/InitialState.h
    src/decorators/QTcpSocketDecorator.h
)

set(LIBRARY_SOURCES
    src/SocksSessionManager.cpp
    src/SocksSession.cpp
    src/SocksServer.cpp
    src/SocksConnection.cpp
    src/decorators/ThrottlingDecorator.cpp
    src/decorators/QIODeviceDecorator.cpp
    src/protocol/SocksReplyMessage4a.cpp
    src/protocol/SocksProtocolMessage.cpp
    src/protocol/Socks5UDPRequestMessage.cpp
    src/protocol/Socks5RequestMessage.cpp
    src/protocol/Socks5ReplyMessage.cpp
    src/protocol/Socks5MethodSelectionMessage.cpp
    src/protocol/Socks5GreetingMessage.cpp
    src/protocol/Socks4RequestMessage.cpp
    src/protocol/Socks4ReplyMessage.cpp
    src/states/SocksState.cpp
    src/states/Socks5UDPAssociateState.cpp
    src/states/Socks5UDPAssociatedState.cpp
    src/states/Socks5ConnectState.cpp
    src/states/Socks5ConnectedState.cpp
    src/states/Socks5AuthState.cpp
    src/states/Socks5AuthenticatedState.cpp
    src/states/Socks4InitialState.cpp
    src/states/Socks4ConnectState.cpp
    src/states/Socks4ConnectedState.cpp
    src/states/InitialState.cpp
    src/decorators/QTcpSocketDecorator.cpp
)

set(SOURCES
    src/main.cpp
    data/SocksProxy.qrc
)

set(TRANSLATIONS
    translation/lang_de.ts
)

if(WIN32)
    # Add application icon
    set(SOURCES ${SOURCES} data/SocksProxy.rc)
endif()

set_source_files_properties(${TRANSLATIONS} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/bin/translations")
qt5_add_translation(QM_FILES ${TRANSLATIONS})

add_executable(SocksProxy WIN32 ${SOURCES} ${LIBRARY_HEADERS} ${LIBRARY_SOURCES} ${QM_FILES})
target_include_directories(SocksProxy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(SocksProxy Qt5::Widgets Qt5::Network)
set_target_properties(SocksProxy PROPERTIES DEBUG_POSTFIX "d")

if(TARGET Qt5::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET SocksProxy
        POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --no-opengl-sw --no-angle --no-system-d3d-compiler --no-compiler-runtime "$<TARGET_FILE_DIR:SocksProxy>/$<TARGET_FILE_NAME:SocksProxy>"
    )
endif()
