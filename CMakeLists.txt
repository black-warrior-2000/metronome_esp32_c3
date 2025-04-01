# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# 定义项目组件列表
set(
    pwm                     # PWM组件（components/pwm）
    # 其他自定义组件（如：uart_component等）
)



project(metronome_esp32_c3)
