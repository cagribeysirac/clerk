# Clerk

## Overview

Clerk connects to MQTT and allows you to subscribe to topics and receive messages. It also inserts the messages into a Postgres/TimescaleDB database.

## Build

### Build Configuration

Create a build directory and generate build configuration files.

```bash
$ cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Build the project

Build and generate the executable file.

```bash
$ cmake --build build
```

Shortcut for build the project.

```bash
$ make build
```

### Run the project

Run the executable file.

```bash
$ ./build/Clerk
```

Shortcut for run the project.

```bash
$ make run
```

## Usage

Example usage:

```bash
$ ./build/Clerk
```

For more information, run:

```bash
$ ./build/Clerk --help
```
