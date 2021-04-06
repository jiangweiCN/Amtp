#!/bin/bash

time=$(date "+%Y%m%d%H%M%S")
installPath="../install"
runFile="amtpsa_64_install_1.0.4.$time.run"
runFilePath="$installPath/$runFile"
tarFile="$installPath/amtpsa_64_install.tar.bz2"
binPath="$installPath/amtpsa_64_bin/"
if [ ! -f "$runFilePath" ];then
	echo "Create installation package------------$runFile"
	echo "Installation package is writing................"
	tar jcvf $tarFile $binPath
	cat _amtpsa_64_install.sh $tarFile > $runFilePath
	rm -rf $tarFile
	echo "Amtpsa_64 installation package completed!"
else
	echo "$runFilePath has exist!"
fi

exit 0



