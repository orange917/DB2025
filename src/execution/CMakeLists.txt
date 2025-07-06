set(SOURCES execution_manager.cpp executor_aggregation.cpp execution_common.cpp)
add_library(execution STATIC ${SOURCES})

# 单独编译execution_common.cpp为静态库，以便其他模块可以直接链接它
add_library(execution_common STATIC execution_common.cpp)

target_link_libraries(execution system record transaction planner)
target_link_libraries(execution_common system record)