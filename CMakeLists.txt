cmake_minimum_required(VERSION 3.22)
project(LaPautaSL VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.4)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(LaPautaSL
    COMPANY_NAME "LaPauta SL"
    PLUGIN_MANUFACTURER_CODE Lapa
    PLUGIN_CODE Lpsl
    FORMATS VST3 Standalone
    PRODUCT_NAME "LaPautaSL"
)

juce_generate_juce_header(LaPautaSL)

target_sources(LaPautaSL PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_link_libraries(LaPautaSL PRIVATE juce::juce_audio_utils juce::juce_dsp)
