#!/bin/bash -e
myRepo=$(pwd)
#CMAKE_GENERATOR_OPTIONS=-G"Visual Studio 16 2019"
#CMAKE_GENERATOR_OPTIONS=-G"Visual Studio 15 2017 Win64"
#CMAKE_GENERATOR_OPTIONS=(-G"Visual Studio 16 2019" -A x64)  # CMake 3.14+ is required
CMAKE_GENERATOR_OPTIONS=(-G"Visual Studio 17 2022" -A x64)
if [  ! -d "$myRepo/opencv"  ]; then
    echo "cloning opencv"
    git clone https://github.com/opencv/opencv.git
else
    cd opencv
    git pull --rebase
    cd ..
fi
if [  ! -d "$myRepo/opencv_contrib"  ]; then
    echo "cloning opencv_contrib"
    git clone https://github.com/opencv/opencv_contrib.git
else
    cd opencv_contrib
    git pull --rebase
    cd ..
fi
RepoSource=opencv
mkdir -p build_opencv
pushd build_opencv
CMAKE_OPTIONS=( \
    -DBUILD_opencv_highgui:BOOL=OFF \
#   -DHIGHGUI_ENABLE_PLUGINS:BOOL=OFF \
    -DBUILD_opencv_python_bindings_generator:BOOL=OFF \
    -DBUILD_opencv_python_tests:BOOL=OFF \
    -DBUILD_JAVA:BOOL=OFF \
    -DBUILD_PERF_TESTS:BOOL=OFF \
    -DBUILD_TESTS:BOOL=OFF \
    -DBUILD_DOCS:BOOL=OFF \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DINSTALL_CREATE_DISTRIB=ON)
set -x
cmake "${CMAKE_GENERATOR_OPTIONS[@]}" "${CMAKE_OPTIONS[@]}" -DOPENCV_EXTRA_MODULES_PATH="$myRepo"/opencv_contrib/modules -DCMAKE_INSTALL_PREFIX="$myRepo/install/$RepoSource" "$myRepo/$RepoSource"
echo 
read -p "Do you want to build OpenCV with the following configuration? (Y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1 # handle exits from shell or function but don't exit interactive shell
fi
echo "************************* $Source_DIR -->debug"
cmake --build .  --config debug
echo "************************* $Source_DIR -->release"
cmake --build .  --config release
echo "************************* install to $myRepo/install/$RepoSource"
cmake --build .  --target install --config release
cmake --build .  --target install --config debug
popd
echo 
read -p "Do you want to copy OpenCV built libraries to Blueprint OBS? (Y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1 # handle exits from shell or function but don't exit interactive shell
fi
cp -r ./install/opencv ./lib/opencv