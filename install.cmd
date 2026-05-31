set INSTALL_DIR=C:\Program Files\libime-libhangul
mkdir "%INSTALL_DIR%"
copy /y "build\bin\Debug\libime-libhangul.dll" "%INSTALL_DIR%"
mkdir "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\hanja.txt" "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\freq-hanja.txt" "%INSTALL_DIR%\hanja"
copy /y "libhangul\data\hanja\freq-hanjaeo.txt" "%INSTALL_DIR%\hanja"
mkdir "%INSTALL_DIR%\data"
copy /y "data\symbols.txt" "%INSTALL_DIR%\data"
regsvr32 /s "%INSTALL_DIR%\libime-libhangul.dll"
pause