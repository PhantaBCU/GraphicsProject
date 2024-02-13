cmake_minimum_required(VERSION 3.6)

project(MeshWireframeDrawer LANGUAGES CXX)

add_executable(MeshWireframeDrawer
    main.cpp

    model.cpp
    model.h

    tgaimage.cpp
    tgaimage.h

    geometry.h
)
