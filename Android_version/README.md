
## Basic instructions from Ray

https://github.com/raysan5/raylib/wiki/Working-for-Android

## My modifications to makefile

`JAVA_HOME			  ?= C:/open-jdk`
`ANDROID_HOME		   = C:\Users\Daniel\AppData\Local\Android\Sdk`
`ANDROID_TOOLCHAIN	   = C:\Users\Daniel\AppData\Local\Android\Sdk\ndk\21.0.6113669\toolchains\llvm\prebuilt\windows-x86_64`
`ANDROID_BUILD_TOOLS	   = $(ANDROID_HOME)/build-tools/28.0.3`

Also the validiy for the certificate was increased from 1000 to 10000 days(more than 25 years). This was required for uploading the apk to Google Play.

## compilation command
`mingw32-make`


## Notes
* There was a problem in my PATH variable in Windows. These entries where making my makefile to fail:

`C:\Program Files\Git\mingw64\bin`
`C:\Program Files\Git\cmd`
`C:\Program Files\Git\usr\bin`


##adb drivers

downloaded from:
https://adb.clockworkmod.com/

##adb commands

C:\Users\Daniel\AppData\Local\Android\Sdk\platform-tools\adb devices  
C:\Users\Daniel\AppData\Local\Android\Sdk\platform-tools\adb -s 4a959c939805 install rayMaschine.apk
C:\Users\Daniel\AppData\Local\Android\Sdk\platform-tools\adb -s 4a959c939805 logcat raylib:V *:S

## Signed and unsiged apk are stored in
C:\GitHub\rayMaschine\Android_version\android.rayMaschine\bin

## Final apk, signed and aligned is the output one:
C:\GitHub\rayMaschine\Android_version\rayMaschine.apk

## keystore management
* **CRITIAL**: Only using makefile each time that we compile the apk, will generate a new mek in a new keystore. THIS IS NOT VALID for Google Play.The request to use always the same keysotre.
 **STILL NOT FIXED IN THE ACTUAL MAKEFILE**
 
* To open existing keystore
C:\open-jdk\bin\keytool -list -v -keystore C:\GitHub\rayMaschine\Android_version\android.rayMaschine\rayMaschine.keystore -storepass rayMaschine

* To add cert to an existing keystore
https://stackoverflow.com/questions/44070168/signing-an-apk-with-an-upload-key-provided-by-google-play

C:\open-jdk\bin\keytool -importcert -file upload_cert.der -keystore rayMaschine.keystore
C:\open-jdk\bin\keytool -list -v -keystore rayMaschine.keystore -storepass rayMaschine
C:\open-jdk\bin\keytool -delete -alias raymaschinekey -keystore rayMaschine.keystore

* Other commands used:

C:/open-jdk/bin/jarsigner -keystore C:\GitHub\rayMaschine\Android_version\rayMaschine.keystore -storepass rayMaschine -keypass rayMaschine -signedjar android.rayMaschine/bin/rayMaschine.signed.apk android.rayMaschine/bin/rayMaschine.unsigned.apk myKey
C:\GitHub\openssl\bin\openssl pkcs12 -export -out newcertbundle.pfx -inkey myprivate.key -in upload_cert.der -name mykey