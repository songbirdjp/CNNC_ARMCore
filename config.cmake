# get git hash
macro(get_git_hash git_hash)
    find_package(Git QUIET)
    if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE ${git_hash}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    endif()
endmacro(get_git_hash)

# get git branch
macro(get_git_branch git_branch)
    find_package(Git QUIET)
    if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE ${git_branch}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    endif()
endmacro(get_git_branch)

# get git tag
macro(get_git_tag git_tag)
    find_package(Git QUIET)
    if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --always --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE ${git_tag}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    endif()
endmacro(get_git_tag)
