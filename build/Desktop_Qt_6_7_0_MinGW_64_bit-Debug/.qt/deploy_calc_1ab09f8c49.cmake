include(C:/Users/mark/Documents/calc/build/Desktop_Qt_6_7_0_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/calc-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "ZlibPrivate;EntryPointPrivate;Core;Gui;Widgets")

qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Users/mark/Documents/calc/build/Desktop_Qt_6_7_0_MinGW_64_bit-Debug/calc.exe
    GENERATE_QT_CONF
)
