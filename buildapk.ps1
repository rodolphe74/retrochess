# desarchivage de l'apk love and le rep apk
.\tools\apktool d -s -o android_build\apk .\tools\love-11.3-android.apk


# si y avait deja un game.love
$file=".\android_build\apk\assets\game.love"
if (test-path -Path $file) {
  Remove-Item $file
}

# copy de la lib compilee sur debian dans le rep de lib pour lua-love
# scp -P10022 rodoc@localhost:/home/rodoc/developpement/love/chesslib/libs/arm64-v8a/libchesslib.so assets\lib\arm64-v8a

# Compress-Archive assets\* -DestinationPath android_build\assets.zip -Update
cd assets
# Remove-Item -Recurse -Force apk
$file="apk"
if (test-path -Path $file) {
  Remove-Item $file
}
..\tools\7z u ..\android_build\assets.zip *
cd ..

$file=".\android_build\apk\assets"
$pathExists = Test-Path -Path $file
if ($pathExists -eq $False) {
  mkdir $file
}

# transforme le repertoire assets en game.love (un zip en fait)
mv android_build\assets.zip android_build\apk\assets\game.love

# scp -P10022 android_build\apk\lib\arm64-v8a  rodoc@localhost:/home/rodoc/developpement/love/helloshared/libs/arm64-v8a/libhello.so
# scp -P10022 rodoc@localhost:/home/rodoc/developpement/love/helloshared/libs/arm64-v8a/libhello.so android_build\apk\lib\arm64-v8a

# reconstruit l'apk avec le game.love dedans
apktool b -o retrochess.apk .\android_build\apk

# et signature de l'apk
java -jar .\tools\uber-apk-signer-1.2.1.jar -a .\retrochess.apk --ks .\tools\ks.jks  --ksAlias ro --ksKeyPass ro --ksPass ro --debug