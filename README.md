# MidiKey

An application which listens for incoming midi messages and forwards predefined key commands to current active X11 window.

By Manuel Oschgan, 2023.

This distribution of MidiKey contains the the main C++ code and a CMakeLists.txt for building the application.

## Building

Create a project for the IDE of your choice using CMake and build the software using the IDE.

Following installed libraries must be present on your system for building:

libxdo (f.e. package libxdo-dev on debian)

rtmidi (librtmidi-dev on debian)

Following libraries will be downloaded automatically by the CPM cmake library:

nlohmann json library

cxxopts library

## Running

In case you do not build the source yourself, one of the two binaries in the release can be used.

One build is usable for Ubuntu 22.04 and one for Ubuntu 20.04 or the corresponding Debian version.

Following dependencies are required for the binaries:

Ubuntu 22.04 binary: librtmidi6, libxdo3

Ubuntu 20.04 binary: librtmidi4, libxdo3

As you can see the rtmidi version has changed between those two Ubuntu versions, therefore the two binaries are supplied.

## Program Parameters

Running application without parameters will output the help.
There are currently only two options:

Parameter1: -p Lists ports of all connected Midi devices

Parameter2: -f Starts listening for incoming messages and sends configured keys to active window

The program will exit the listening mode after pressing enter.

## Program Options

Program options have to be specified using the MidiKey JSON format config file. An example config is provided in the repository
in the form of config_example.json.

MidiKey will look for the file named config.json in its current directory. Please modify the example config to suit your needs
and rename/copy to the correct destination.

First option in the config file is the listening port. This is the port the incoming messages will be received. The correct port of your
device can be retrieved by launching the application with the -p switch.

Second option is the message map. Message map is an json array where each item contains a message and key specification.
Message is the midi message represented as array of bytes. Key specification is a string which is send to the active window
upon reception of the midi message. See libxdo/xdotools for details about the key specification.

## Legal and ethical

The MidiKey license is similar to the MIT License, with the added *feature* that modifications be sent to the developer.  Please see [LICENSE](LICENSE).
