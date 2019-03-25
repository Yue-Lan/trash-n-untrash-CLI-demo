# trash-n-untrash-CLI-demo
  An easy console application for trash file and restore file through comand line.

# build
It mostly depends on glib and gio, so there is easy to build.

This program build by qmake, but it just use qDebug for show the message on terminal.

You can build it by qt-creator, or just use qmake.

- NOTE:

    Trash is an virtral file system, which is provided by gvfs. There may be an native path for trash in .local/share/Trash or other place, but restore operation is based on the uri schema "trash://". So, restore a file just support use trash schema (do not use 'file:///home/xxx/.local/share/Trash/files/xxx')/

# how to use

  After build, we open the terminal, and run "./$(BUILD_PATH)/cmd_demo -h" to get help of this program.
  Use --list-trash to list all files' names in trash.
  Use --trash-file to send a file to trash.
  Use --restore-file to restore a file from trash.
  Use --show-orig to show the original path of the trashed file before it trashed.
