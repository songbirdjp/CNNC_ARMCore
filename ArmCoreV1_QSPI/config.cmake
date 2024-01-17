# get git hash
macro(get_git_hash git_hash)
    find_package(Git QUIET)
    if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H
        OUTPUT_VARIABLE ${git_hash}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    endif()
endmacro(get_git_hash)

# get it branch
macro(get_git_branch git_branch)
    find_package(Git QUIET)
    if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        OUTPUT_VARIABLE ${git_branch}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    endif()
endmacro(get_git_branch)


