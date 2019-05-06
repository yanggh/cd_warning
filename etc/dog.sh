ps -ef |grep Demo |grep -v grep
if [ $? -eq 0 ];then
echo "Demo running"
else
cd /home/jzgj/crscd/bin/
date  >> /home/jzgj/crscd/logs/log
echo "Demo starting" >> /home/jzgj/crscd/logs/log
./Demo
fi

ps -ef |grep tomcat |grep -v grep
if [ $? -eq 0 ];then
echo "tomcat running"
else
date  >> /home/jzgj/crscd/logs/log
echo "tomcat starting" >> /home/jzgj/crscd/logs/log
cd /home/jzgj/crscd/tomcat/bin/
./startup.sh
fi

cd  /home/jzgj/crscd/logs
rm  -f catalina*.out
rm  -f syslog*
cp  /home/jzgj/crscd/tomcat/logs/catalina*.out   /home/jzgj/crscd/logs/
cp  /var/log/syslog   /home/jzgj/crscd/logs/
rm  -f     log.tar.gz
tar -czvf  log.tar.gz  *
rm  -f catalina*.out
rm  -f syslog*

cd /home/jzgj/crscd/logs/pcap/
find . -iname  `date +%Y%m%d%H --date="-7 day"`.pcap  | xargs rm -f
pid=`ps -ef | grep -v grep | grep  tcpdump | awk '{print $2}'`
if [ $pid -gt 0 ]
then
echo "sdfasdfasd"
else
tcpdump -i eth3 udp port 18888 -s0 -G 3600 -Z root -w %Y%m%d%H.pcap  &
fi

#/usr/bin/find /home/jzgj/crscd/tomcat/logs/ -iname catalina.out -size +50M  -exec cp {} /home/jzgj/crscd/tomcat/logs/catalina`date +%Y%m%d%H%M`.out  \;
#/usr/bin/find /home/jzgj/crscd/tomcat/logs/ -iname catalina.out -size +50M  -exec echo "">/home/jzgj/crscd/tomcat/logs/catalina.out \;
#/usr/bin/find /home/jzgj/crscd/tomcat/logs/ -iname catalina*.out -mtime  +15    -exec rm -rf {}  \;
