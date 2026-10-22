# Godot 4.7.1 iOS shell

CI builds an unsigned (ldid fakesigned) iOS app containing only the official Godot 4.7.1 iOS engine,
a tiny placeholder project (`shell/`) and a native crash logger (`native/crash_handler.inc`, writes
`Documents/KILLSEC-iOS-crash.log`). No game content is stored or used in this repository.
