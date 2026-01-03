C++:
- Qt style: QMainWindow subclass, setupUI() helper, signal/slot connections.
- Headers use include guards (ALL_CAPS).
- Pointer members use m_ prefix (m_button).
- Uses C++17 and Qt parent-child ownership for cleanup.

Python:
- PEP8-ish style with docstrings and type hints.
- Uses standard library only (json, sys, typing).

CMake:
- Modern CMake with target_link_libraries and find_package(Qt6, Python3).

No explicit lint/format rules or testing framework found.