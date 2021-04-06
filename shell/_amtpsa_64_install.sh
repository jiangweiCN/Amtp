#!/bin/bash
lines=26
install_path="/usr/local/amtpsa"
install_tar="amtpsa_64_install.tar.gz"
tail -n +$lines $0 > $install_tar
tar jxvf $install_tar
mkdir $install_path
chmod 777 $install_path
cp -r install/amtpsa_64_bin/* $install_path
rm -rf install
rm -f $install_tar
cp $install_path/libprotobuf.so.22.0.2 /usr/lib64/
ln -snf /usr/lib64/libprotobuf.so.22.0.2  /usr/lib64/libprotobuf.so.22
cp $install_path/libzmq.so.5.2.1 /usr/lib64/
ln -snf /usr/lib64/libzmq.so.5.2.1  /usr/lib64/libzmq.so.5
cp $install_path/libstdc++.so.6.0.25 /usr/lib64/
ln -snf /usr/lib64/libstdc++.so.6.0.25 /usr/lib64/libstdc++.so.6
cp $install_path/libglog.so.0.0.0 /usr/lib64/
ln -snf /usr/lib64/libglog.so.0.0.0 /usr/lib64/libglog.so.0
cp $install_path/libjwumq.so.1.0.3 /usr/lib64
ln -snf /usr/lib64/libjwumq.so.1.0.3  /usr/lib64/libjwumq.so
cp $install_path/libamtpsa.so.1.0.4 /usr/lib64
ln -snf /usr/lib64/libamtpsa.so.1.0.4  /usr/lib64/libamtpsa.so
echo "Amtpa installation package completed!"
exit 0
