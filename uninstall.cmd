set INSTALL_DIR=C:\Program Files\libime-libhangul
regsvr32 /u /s "%INSTALL_DIR%\libime-libhangul.dll"
del "%INSTALL_DIR%\libime-libhangul.dll"
rmdir /s /q "%INSTALL_DIR%"
pause