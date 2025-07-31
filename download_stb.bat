@echo off
echo Downloading stb_image.h for image loading...
powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/nothings/stb/master/stb_image.h' -OutFile 'stb_image.h'"
echo Done! stb_image.h downloaded.
pause
