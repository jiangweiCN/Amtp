#!/bin/bash

time=$(date "+%Y%m%d%H%M%S")
installPath="../install"
runFile="amtpca_64_install_1.1.1.$time.run"
runFilePath="$installPath/$runFile"
tarFile="$installPath/amtpca_64_install.tar.bz2"
binPath="$installPath/amtpca_64_bin/"
if [ ! -f "$runFilePath" ];then
	echo "Create installation package------------$runFile"
	echo "Installation package is writing................"
	tar jcvf $tarFile $binPath
	cat _amtpca_64_install.sh $tarFile > $runFilePath
	rm -f $tarFile
	echo "Amtpca_64 installation package completed!"
else
	echo "$runFilePath has exist!"
fi

exit 0



