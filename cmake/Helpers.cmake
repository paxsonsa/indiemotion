macro(helpers_list_directory result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      if(NOT ${child} STREQUAL "_template")
        list(APPEND dirlist ${child})
      endif()
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()
