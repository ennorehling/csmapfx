# Running on macOS

Since wine no longer runs 32 bit windows applications, the easy method
of running the windows executable with wine is broken.

## Docker

We can run the linux executable in docker. assuming a docker image that
can run csmapfx, we need to install XQuartz and socat from brew.

Log out and back in after installing XQuartz.
First, make sure nothing is listening on port 6000: 
```
$ lsof -i TCP:6000
```

Then, run socat:
```
$ socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\"
```
open a new terminal. run XQuartz, and in the preferences, under Security, make sure that "Allow connections from network clients" is checked.

now, run docker with the correct DISPLAY variable:
```
$ docker run -v /Users/enno/Dropbox/Eressea:/data -d -e DISPLAY=docker.for.mac.host.internal:0 -i --name csmapfx eressea/csmapfx-dev
$ docker exec -it csmapfx /usr/games/csmapfx
```

