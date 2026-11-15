cmake_minimum_required(VERSION 3.24)
project(PedalLegends VERSION 0.3.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(PEDAL_LEGENDS_BUILD_TESTS "Build portable DSP tests" ON)
option(PEDAL_LEGENDS_BUILD_PLUGIN "Build JUCE AU/VST3 plugin" ON)
add_library(PedalLegendsDSP STATIC Source/PedalDSP.cpp Source/FactoryPresets.cpp)
target_include_directories(PedalLegendsDSP PUBLIC Source)
set_target_properties(PedalLegendsDSP PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(PedalLegendsDSP PUBLIC cxx_std_17)
if(PEDAL_LEGENDS_BUILD_TESTS)
  enable_testing()
  add_executable(PedalLegendsDSPTests Tests/test_dsp.cpp)
  target_link_libraries(PedalLegendsDSPTests PRIVATE PedalLegendsDSP)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(PedalLegendsDSPTests PRIVATE -UNDEBUG)
  endif()
  add_test(NAME PedalLegendsDSP COMMAND PedalLegendsDSPTests)
  find_package(Python3 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND)
    add_test(NAME PedalLegendsUILocalization
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_ui_localization.py")
    add_test(NAME PedalLegendsRoutingLayout
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_routing_layout.py")
  endif()
endif()
if(PEDAL_LEGENDS_BUILD_PLUGIN)
  find_package(JUCE 8 CONFIG QUIET)
  if(NOT JUCE_FOUND)
    include(FetchContent)
    FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.10 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
  endif()
  if(APPLE)
    set(PLUGIN_FORMATS AU VST3)
  else()
    set(PLUGIN_FORMATS VST3)
  endif()
  juce_add_plugin(PedalLegends
    COMPANY_NAME "Liberty Music Software"
    BUNDLE_ID "com.libertymusicsoftware.pedallegends"
    PLUGIN_MANUFACTURER_CODE Lbty
    PLUGIN_CODE PdLg
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "PEDAL LEGENDS"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)
  juce_generate_juce_header(PedalLegends)
  target_sources(PedalLegends PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/UILocalization.h
    Source/FactoryPresets.h)
  target_link_libraries(PedalLegends PRIVATE
    PedalLegendsDSP juce::juce_audio_utils juce::juce_dsp
    juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
  if(MSVC)
    target_compile_options(PedalLegends PRIVATE /utf-8)
  endif()
  target_compile_definitions(PedalLegends PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
endif()
