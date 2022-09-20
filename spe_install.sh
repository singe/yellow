#!/bin/sh

# Install a "Sensitive Process Execution" canary token for Linux
# by @singe

dns="$1" # The DNS FQDN of your canary token e.g. m7trogdorbrt3.canarytokens.com
process="$2" # The full path of the process you want to trigger, it doesn't need to exist
ext=".yellow"

yellow="/bin/yellow"
echo """#!/bin/sh
#u=\$(id -u):\$(id -g)
u=u\$(whoami | sed \"s/[^\\\x00-\\\x7f]//g\" | head -c 64 )
c=c\$(hostname | sed \"s/[^\\\x00-\\\x7f]//g\")
data=\$c.UN.\$u.CMD.$dns
#getent hosts \$data > /dev/null
if [ ! -e \$0$ext ]; then
  exit
fi
if [ \$(readlink -f \$0$ext) = \"/bin/busybox\" ]; then
  /bin/busybox \$0
else
  \$0$ext
fi""" > $yellow
chmod +x $yellow
if [ -e $process ]; then
  mv $process $process$ext
fi
ln -s $yellow $process
