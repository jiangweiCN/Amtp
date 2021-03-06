#!/bin/bash
lines=28
install_path="/usr/local/jwumq"
install_tar="jwumq_install.tar.gz"
tail -n +$lines $0 > $install_tar
tar jxvf $install_tar
mkdir $install_path
chmod 777 $install_path
cp -r install/jwumq_bin/* $install_path
rm -rf install/jwumq_bin
rm -f $install_tar
cp $install_path/libglog.so.0.0.0 /usr/lib64/
ln -snf /usr/lib64/libglog.so.0.0.0 /usr/lib64/libglog.so.0
cp $install_path/libjwumqa.cpython-36m-x86_64-linux-gnu.so /usr/lib64
cp $install_path/libjwumqa_plugin.so /usr/lib64/
cp $install_path/libjwumqa.so.2.1.0 /usr/lib64
ln -snf /usr/lib64/libjwumqa.so.2.1.0  /usr/lib64/libjwumqa.so
cp $install_path/libjwumq.so.1.0.3 /usr/lib64
ln -snf /usr/lib64/libjwumq.so.1.0.3  /usr/lib64/libjwumq.so
cp $install_path/libprotobuf.so.22.0.2 /usr/lib64/
ln -snf /usr/lib64/libprotobuf.so.22.0.2  /usr/lib64/libprotobuf.so.22
cp $install_path/libstdc++.so.6.0.25 /usr/lib64/
ln -snf /usr/lib64/libstdc++.so.6.0.25 /usr/lib64/libstdc++.so.6
cp $install_path/libzmq.so.5.2.1 /usr/lib64/
ln -snf /usr/lib64/libzmq.so.5.2.1  /usr/lib64/libzmq.so.5
echo "Jwumq installation package completed!"
exit 0
