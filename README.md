# graphcalc2
A calculator app with rendering graphs in real time

# Graphical calculator with graphs support

This project is a simple calculator that has support for rendering graphs of functions in real time.
[calculator picture](misc/claculator.png)

# How to build
## Requirements
- qt6-qmake
- libqt6printsupport
- qt6
- make
- gcc
- stdc++ libs (usually bundled with g++)

## build steps
```
cd src
make
```

If you want to statically build the application (without Qt dynamic lib dependencies), you can [add statically configured qt to PATH](https://doc.qt.io/qt-6/linux-deployment.html).
