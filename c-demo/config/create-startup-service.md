```sh
cd /usr/bin/
nano gps.sh
```

```
#!/bin/bash
/home/root/name_of_compiled_code
```


sudo nano /etc/rc.local

```sh
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

cd /home/debian/BeagleBone-Black/c-demo/poc/12-startup
/home/debian/BeagleBone-Black/c-demo/poc/12-startup

exit 0
```

tail -f /proc/<pid>/fd/1