# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

LinuxTimelineViewer is a Qt5.15.13-based C++ application for visualizing large forensic timeline CSV files in a tabbed GUI. It's designed for digital forensics investigators to efficiently handle multi-GB timeline files without loading them entirely into memory.

## Common Commands

### Build System
**Note: This project is designed for Linux/Unix environments. Building requires a Linux system with Qt5.15.13+ and CMake.**

```bash
# Create build directory and configure (Linux only)
mkdir build && cd build
export CMAKE_PREFIX_PATH=/opt/Qt5.15.13-static/lib/cmake
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC=ON

# Build the application (Linux only)
make -j$(nproc)

# The binary will be output to: build/bin/LinuxTimelineViewer
```

### Development and Testing
```bash
# Run the application with test files (Linux only)
./build/bin/LinuxTimelineViewer data/test_files/FILESYSTEM.txt
./build/bin/LinuxTimelineViewer data/test_files/SUPER.txt

# Run without arguments to use File → Open menu (Linux only)
./build/bin/LinuxTimelineViewer
```

### Windows Development Environment Note
This codebase cannot be built or run on Windows systems. It requires Qt5.15.13+ and a Linux/Unix build environment. The application is specifically designed for forensic analysts working on Linux systems.

### Git Configuration
**IMPORTANT**: This user account has GPG signing enabled. All git commits must use the `-S` flag for signing:
```bash
git commit -S -m "commit message"
```

## Architecture and Structure

### Core Components

**AppWindow** (`src/AppWindow.h/.cpp`): Main application window
- Manages QTabWidget for multiple timeline files
- Handles File menu (Open, Exit) and View menu (font sizing, search)
- Coordinates font size and line height changes across all tabs
- Provides search functionality across current tab or all tabs

**TimelineTab** (`src/TimelineTab.h/.cpp`): Individual tab for each loaded file
- Contains FilterBar (search UI), QTableView, and status bar
- Manages TimelineModel and QSortFilterProxyModel for data display
- Handles column-based filtering and search highlighting
- Supports font size and line height adjustments
- Double-click on any cell opens FieldDetailWindow for expanded view

**TimelineModel** (`src/TimelineModel.h/.cpp`): QAbstractTableModel implementation
- File-backed model that doesn't load entire CSV into memory
- Uses line offset indexing for efficient random access
- Supports two timeline formats:
  - Filesystem: `Date,Size,Type,Mode,UID,GID,Meta,File Name`
  - Super: `datetime,timestamp_desc,source,source_long,message,parser,display_name,tag`
- Auto-detects format from CSV headers using TimelineParser

**TimelineParser** (`src/TimelineParser.h/.cpp`): Format detection utilities
- Static methods for identifying timeline format from headers
- Validates column structure against known timeline types

**FilterBar** (`src/FilterBar.h/.cpp`): Search and filter UI widget
- Column picker dropdown, search input field, and search button
- Emits searchRequested signals for TimelineTab to handle

**FieldDetailWindow** (`src/FieldDetailWindow.h/.cpp`): Popup dialog for field content
- Resizable window displaying full content of a timeline field
- Triggered by double-clicking any cell in the timeline table
- Uses monospace font for optimal XML/JSON display
- Non-modal to allow multiple detail windows simultaneously

**Utils Components**:
- **JsonXmlFormatter** (`src/utils/JsonXmlFormatter.h/.cpp`): Automatically detects and pretty-prints embedded JSON/XML in Super timeline message fields. Applied during data display in TimelineModel for column 4 (message field).
- **FileUtils** (`src/utils/FileUtils.h/.cpp`): File path utilities and proper CSV parsing that handles quoted fields with embedded commas

### Key Design Patterns

**Memory Efficiency**: Uses file offset indexing rather than loading entire CSV files into memory, allowing handling of multi-GB timeline files.

**Format Detection**: Automatically detects timeline format (Filesystem vs Super) from CSV headers and adapts column layout accordingly.

**Modular Search**: FilterBar component provides reusable search UI that can be easily extended or modified.

**Qt Model/View**: Leverages Qt's MVC architecture with QAbstractTableModel and QSortFilterProxyModel for data display and filtering.

### Build Requirements

- C++17 compiler
- CMake 3.16+
- Qt 5.15.13 (preferably static build for distribution)
- Qt modules: Widgets, Gui, Core, Xml

### Test Data

Test files are located in `data/test_files/`:
- `FILESYSTEM.txt`: Example filesystem timeline format
- `SUPER.txt`: Example Plaso supertimeline format

Both formats are auto-detected by the application when loaded.