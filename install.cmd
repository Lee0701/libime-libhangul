set INSTALL_DIR=C:\Program Files\libime-libhangul
mkdir "%INSTALL_DIR%"
mkdir "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\hanja.txt" "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\freq-hanja.txt" "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\freq-hanjaeo.txt" "%INSTALL_DIR%\hanja"
copy /y "build\bin\Debug\libime-libhangul.dll" "%INSTALL_DIR%"
regsvr32 /s "%INSTALL_DIR%\libime-libhangul.dll"
pause