#!/bin/bash

time=$(date "+%Y%m%d%H%M%S")
installPath="../install"
runFile="jwumq_install_2.0.1.$time.run"
runFilePath="$installPath/$runFile"
tarFile="$installPath/jwumq_install.tar.bz2"
binPath="$installPath/jwumq_bin/"
if [ ! -f "$filePath" ];then
	echo "Create installation package------------$runFile"
	echo "Installation package is writing................"
	tar jcvf $tarFile $binPath
	cat _jwumq_install.sh $tarFile > $runFilePath
	rm -f $tarFile
	echo "Jwumq installation package completed!"
else
	echo "$runFilePath has exist!"
fi

exit 0



