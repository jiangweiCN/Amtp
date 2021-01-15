#!/bin/bash

time=$(date "+%Y%m%d%H%M%S")
installPath="../install"
runFile="amtpa_install_1.0.1.$time.run"
runFilePath="$installPath/$runFile"
tarFile="$installPath/amtpa_install.tar.bz2"
binPath="$installPath/amtpa_bin/"
if [ ! -f "$runFilePath" ];then
	echo "Create installation package------------$runFile"
	echo "Installation package is writing................"
	tar jcvf $tarFile $binPath
	cat _amtpa_install.sh $tarFile > $runFilePath
	rm -f $tarFile
	echo "Amtpa installation package completed!"
else
	echo "$runFilePath has exist!"
fi

exit 0



