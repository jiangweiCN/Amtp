#!/bin/bash
lines=26
install_path="/usr/local/amtpca"
install_tar="amtpca_32_install.tar.gz"
tail -n +$lines $0 > $install_tar
tar jxvf $install_tar
mkdir $install_path
chmod 777 $install_path
cp -r install/amtpca_32_bin/* $install_path
rm -rf install
rm -f $install_tar
cp $install_path/libprotobuf.so.22.0.2 /usr/lib/
ln -snf /usr/lib/libprotobuf.so.22.0.2  /usr/lib/libprotobuf.so.22
cp $install_path/libzmq.so.5.2.1 /usr/lib/
ln -snf /usr/lib/libzmq.so.5.2.1  /usr/lib/libzmq.so.5
cp $install_path/libstdc++.so.6.0.25 /usr/lib/
ln -snf /usr/lib/libstdc++.so.6.0.25 /usr/lib/libstdc++.so.6
cp $install_path/libglog.so.0.0.0 /usr/lib/
ln -snf /usr/lib/libglog.so.0.0.0 /usr/lib/libglog.so.0
cp $install_path/libjwumq.so.1.0.3 /usr/lib
ln -snf /usr/lib/libjwumq.so.1.0.3  /usr/lib/libjwumq.so
cp $install_path/libamtpca.so.1.0.1 /usr/lib
ln -snf /usr/lib/libamtpca.so.1.0.1  /usr/lib/libamtpca.so
echo "Amtpca_32 installation package completed!"
exit 0
