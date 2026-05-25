# Allow the user to point at a local copy or a different remote
if(NOT DEFINED MAVLINK_URL)
  set(MAVLINK_URL "https://github.com/mavlink/c_library_v2.git")
endif()

if(NOT DEFINED MAVLINK_TAG)
  set(MAVLINK_TAG "7371b9feb5e69cd430b43ee32e194482fadbf5ff")
endif()

include(FetchContent)

FetchContent_Declare(
  mavlink
  GIT_REPOSITORY ${MAVLINK_URL}
  GIT_TAG        ${MAVLINK_TAG}
  # Can be enabled if we used actual tags or branches
  # GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(mavlink)

target_include_directories(zephyr_interface SYSTEM INTERFACE
  ${mavlink_SOURCE_DIR}           # FetchContent exposes this variable automatically
  ${CMAKE_CURRENT_BINARY_DIR}     # for the generated shim
)

if(NOT CONFIG_MAVWRAP_DIALECT_NAME)
  message(FATAL_ERROR "No MAVLink dialect selected. Set CONFIG_MAVWRAP_DIALECT_NAME in your prj.conf.")
endif()

# Generate a shim that includes the right dialect
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/mavlink_dialect.h
  "#include <${CONFIG_MAVWRAP_DIALECT_NAME}/mavlink.h>\n"
)

if(CONFIG_MAVWRAP)

zephyr_library()

zephyr_include_directories(include)

zephyr_library_include_directories(src)

zephyr_library_sources(
    src/mavwrap_core.c
)

zephyr_library_sources_ifdef(CONFIG_MAVWRAP_TRANSPORT_UART
    src/mavwrap_uart.c
)

zephyr_library_sources_ifdef(CONFIG_MAVWRAP_TRANSPORT_LORA
    src/mavwrap_lora.c
)

zephyr_library_sources_ifdef(CONFIG_MAVWRAP_TRANSPORT_NETIF
    src/mavwrap_netif.c
    src/mavwrap_udp.c
)

endif() # CONFIG_MAVWRAP
