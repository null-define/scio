if(NOT scio_LINK_THIRD_PARTY_OPTION)
      find_package(fmt CONFIG REQUIRED)
      find_package(Boost COMPONENTS system thread)
      # add coroutine ts
      add_library(std::coroutines INTERFACE IMPORTED)
      target_compile_options(std::coroutines INTERFACE "-fcoroutines-ts")
      # add third party libs
      add_library(scio::third_party INTERFACE IMPORTED)
      target_link_libraries(scio::third_party INTERFACE 
      std::coroutines 
      fmt::fmt 
      pthread)
      set(scio_LINK_THIRD_PARTY_OPTION TRUE)
endif()