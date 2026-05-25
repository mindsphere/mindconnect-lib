set openssl_version=3.5.2
set curl_version=8.16.0
set path_to_7zip="C:\Program Files\7-Zip"
set path_to_visual_studio="C:\apps\MVS16"
set main_directory=..\mcl_sandbox
set mcl_directory=%cd%

mkdir %main_directory%
cd %main_directory%

rem Change relative path to absolute.
set main_directory="%cd%"

powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls13; wget https://www.openssl.org/source/openssl-%openssl_version%.tar.gz -OutFile openssl-%openssl_version%.tar.gz}"
%path_to_7zip%\7z e openssl-%openssl_version%.tar.gz
%path_to_7zip%\7z x openssl-%openssl_version%.tar

powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls13; wget https://curl.se/download/curl-%curl_version%.tar.gz -OutFile curl-%curl_version%.tar.gz}"
%path_to_7zip%\7z e curl-%curl_version%.tar.gz
%path_to_7zip%\7z x curl-%curl_version%.tar

set path_to_visual_studio_build=%path_to_visual_studio%\VC\Auxiliary\Build

if not defined DevEnvDir (call %path_to_visual_studio_build%\vcvars64.bat)

cd openssl-%openssl_version%
perl Configure VC-WIN64A --prefix=%main_directory%\install no-asm
nmake install_sw
cd ..

cd curl-%curl_version%\winbuild
nmake /f Makefile.vc WINBUILD_ACKNOWLEDGE_DEPRECATED=yes mode=dll VC=16 WITH_SSL=dll MACHINE=x64 WITH_PREFIX=%main_directory%\install\ SSL_PATH=%main_directory%\install
cd ../..

mkdir mcl_build
cd mcl_build
cmake -DCMAKE_PREFIX_PATH="%main_directory%/install" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX:PATH=%main_directory%/install -DMCL_STATICLIB=OFF -DMCL_LOG_LEVEL=MCL_LOG_LEVEL_NONE %mcl_directory%
cmake --build . --clean-first --target install
