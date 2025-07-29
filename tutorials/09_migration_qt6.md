\page migration_qt6 Migrating Qt5 GUI plugins to Qt6

Prerequisite Tutorial: \ref plugins

## Overview

Qt5 is planned to be removed in Ubuntu 26.04 LTS. In response to this,
Gazebo GUI has upgraded its GUI framework from Qt5 to Qt6 in `gz-gui10`
(Gazebo Jetty). This directly impacts all users who have developed
their own custom Gazebo GUI plugins in Qt5.

This tutorial will highlight important changes that users need to make
to migrate their existing Qt5 Gazebo GUI plugins to Qt6.

A Gazebo GUI is made up of C++, QML code, and configuration file, see
the \ref plugins tutorial for more information. We expect that most of the
required changes for porting to Qt6 will be in the QML code.

## QML and C++ Integration

In a Gazebo GUI plugin, users can create a QML interface that invokes C++
functionality and vice versa. There is one major syntax change to how
this should be done from `gz-gui10` onwards. Prior to `gz-gui10`, access to
C++ functions or properties was done with the following syntax in QML:

```qml
MyClass.FunctionFoo()
```

The new syntax for doing this in Qt6 based Gazebo GUI plugin is:

```qml
_MyClass.FunctionFoo()
```

As an example, in the Qt5 based `Screenshot` GUI plugin's QML code, we call
[Screenshot.OnScreenshot()](https://github.com/gazebosim/gz-gui/blob/e0c95585919d95a01fbf3af9a33c7fcd653ab154/src/plugins/screenshot/Screenshot.qml#L50)
which inovkes the corresponding [C++ function](https://github.com/gazebosim/gz-gui/blob/e0c95585919d95a01fbf3af9a33c7fcd653ab154/src/plugins/screenshot/Screenshot.cc#L188)
defined in the `Screenshot` C++ class.

In the Qt6 based Gazebo GUI plugin, the code is changed to
[\_Screenshot.OnScreeShot()](https://github.com/gazebosim/gz-gui/blob/c7093159ac92031350cdf2c31283e3fdfd944194/src/plugins/screenshot/Screenshot.qml#L50)
for the QML and C++ binding to work correctly.

This is mainly because Qt6 enforces stricter property rules and that property
names should not begin with an upper case letter. Gazebo adds an
underscore prefix to workaround this problem. No changes should be needed on
the C++ end.

> Note: An alternative approach would be to choose a name that starts
> with a lower case letter, i.e. `myClass.FunctionFoo()`. However, it was
> noted that in many cases there exists an QML object with the id `myClass`
> already which would conflict with the name of the C++ object. For example, the
> Screenshot GUI plugin has a tool button with the id:
> [screenshot](https://github.com/gazebosim/gz-gui/blob/c7093159ac92031350cdf2c31283e3fdfd944194/src/plugins/screenshot/Screenshot.qml#L37).
> Hence a decision was made to add the `_` prefix to avoid any potential
> naming conflicts.

## QML Migration

Gazebo gives users the freedom to import any QML modules in their QML code
and create their UI in a way that is no different from writing other Qt
programs. So users would need to follow the general
[porting guide](https://doc.qt.io/qt-6/portingguide.html), paying attention
to the QML and module changes. Some QML modules and types are obsolete while
some QML types have significant syntax changes.

Common changes for porting Qt5 QML code to Qt6 include:
* Importing a newer version of the Qt module, which can be done by removing the
  version number, e.g. `import QtQuick.Dialogs 1.0` becomes
  `import QtQuick.Dialogs`.
* Adding the `_` prefix to the C++ object name as described in the
  `QML and C++ Integration` section above.
* QML types requiring more code changes include but not limited to:
  [FileDialog](https://doc.qt.io/qt-6/qml-qtquick-dialogs-filedialog.html) and
  [TreeView](https://doc.qt.io/qt-6/qml-qtquick-treeview.html).
  *  This [Qt6 migration pull request](https://github.com/gazebosim/gz-sim/pull/2832/files#diff-a93324029765acbdf791f6e6ed06b1ea2e2886a756f949e7948f7824a57b4e7b)
    shows examples of porting various Qt5 Gazebo GUI plugins to Qt6.

## Qt App and Event Loop

The core of Gazebo GUI sets up the necessary code for running an Qt application
and its event loop. Here are some related migration notes:

* The main C++ API in Gazebo GUI for retrieving the running Qt Application
is [App()](https://github.com/gazebosim/gz-gui/blob/e0c95585919d95a01fbf3af9a33c7fcd653ab154/include/gz/gui/Application.hh#L224)
which essentially returns a
[qGuiApp](https://doc.qt.io/qt-6/qguiapplication.html#qGuiApp) pointer.
This pointer is no longer always guaranteed to be non-null. Users should check this
pointer to make sure it is valid before using it.
* Users are discouraged from calling
[QCoreApplication::processEvents](https://doc.qt.io/qt-6/qcoreapplication.html#processEvents)
manually in Qt6.
