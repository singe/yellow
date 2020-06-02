```
            _ _               
 _   _  ___| | | _____      __
| | | |/ _ \ | |/ _ \ \ /\ / /
| |_| |  __/ | | (_) \ V  V / 
 \__, |\___|_|_|\___/ \_/\_/  
 |___/               
```
Just add blue.

# yellow - just add blue

A simple binary wrapper that will trigger a [canarytoken](https://canarytokens.org/") when a binary is executed.

Ideal for boobytrapping production systems.

Currently there are two methods, yellow is a targetted approach with a binary that will trigger the token then execute the real binary based on a file extension of your choosing. libyellow is a cross-system notification and will transparently trigger a canarytoken for any binary when it's injected via LD_PRELOAD.

## Pre-work

Register a DNS canary token of your own at [canarytokens](https://canarytokens.org/"). You can read more about how they work at [the docs](https://docs.canarytokens.org/guide/dns-token.html).

It will looks something like this: `pz21qtyfsidipvrsuzs9n2udi.canarytokens.com`

Put it into the environmental variable TOKEN with:
`export TOKEN="pz21qtyfsidipvrsuzs9n2udi.canarytokens.com"`

## Building

If you have a suitable default built environment (e.g. build-essential on debian based systems or build-base on alpine based systems), you can compile libyellow or yellow very simply.

### yellow

`gcc -o yellow yellow.c canary32.c`

### libyellow

`gcc -shared -fPIC libyellow.c canary32.c -o libyellow.so`

## Installing

### yellow

yellow is intended to trigger on specific binaries you rename and alias to it, much like how busybox is used. You'll need to rename the binary to use the ".canary" extension, then create a symlink (or copy) of yellow for the original binary name. For example:

```
cp yellow /usr/bin
mv /usr/bin/id /usr/bin/id.canary
ln -s /usr/bin/yellow /usr/bin/id
```

After this any execution of `id` will trigger your canary token.

### libyellow

libyellow is intended to trigger on any binary used where it has been injected via LD_PRELOAD. This can be done per session by setting the LD_PRELOAD environment variable to point to it, or across the system by adding it into the `/etc/ld.so.preload` file. The latter can break your system if the library isn't working on it, so it's recommended you test it using LD_PRELOAD first.

This will be noisy i.e. you'll get a lot of alerts, see yellow for a more targetted aproach.

Remember to put your token in the TOKEN environment variable as described under "Pre-Work" above.

LD_PRELOAD example:
```
cp libyellow.so /usr/lib/
export LD_PRELOAD=/usr/lib/libyellow.so
```
Then run any binary, check that it behaves as expected, and you get a canary alert.

Once you're happy everything is working, you can add it across the system with:
```
cp libyellow.so /usr/lib
echo /usr/lib/libyellow.so >> /etc/ld.so.preload
```
