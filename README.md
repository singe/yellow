```
            _ _               
 _   _  ___| | | _____      __
| | | |/ _ \ | |/ _ \ \ /\ / /
| |_| |  __/ | | (_) \ V  V / 
 \__, |\___|_|_|\___/ \_/\_/  
 |___/               
 - Just add blue.
```

# yellow - just add blue

Simple binary modifications that will trigger a [canarytoken](https://canarytokens.org/") when a binary is executed.

Ideal for boobytrapping production systems against attackers.

Currently there are four methods:

* yellow is a targetted approach with a binary that will trigger the token then execute the real binary based on a file extension of your choosing. 
* spe is a variation on the above using a similar approach to Thinkst's recently added ["Sensitive Process Execution"](https://blog.thinkst.com/2022/09/sensitive-command-token-so-much-offense.html) token for Windows.
* libyellow is a cross-system notification and will transparently trigger a canarytoken for any binary name you specify when it's injected via LD_PRELOAD
* ldsoyellow is a fully functional glibc linker (ld.so) that will trigger a canarytoken for any binary that uses it

## Pre-work

Register a DNS canary token of your own at [canarytokens](https://canarytokens.org/"). You can read more about how they work at [the docs](https://docs.canarytokens.org/guide/dns-token.html).

It will looks something like this: `pz21qtyfsidipvrsuzs9n2udi.canarytokens.com`

Put it into the environmental variable TOKEN with:
`export TOKEN="c28y9l4dw0drj62un0cm4rwz6.canarytokens.com"`

The Dockerfiles have an example, working token in them, you can check its activity history [here](https://canarytokens.org/history?token=c28y9l4dw0drj62un0cm4rwz6&auth=4bdd0b79ce575d6b1e2d1c90aec5b5ad). You really should use your own though.
 
## Building

Dockerfiles implementing each of these build methods are available.

If you have a suitable default build environment (e.g. build-essential on debian based systems or build-base on alpine based systems), you can compile libyellow or yellow very simply. ldsoyellow is more involved.

You can see simple end-to-end example of Building and Installing yellow in the `Dockerfile`s.

### yellow

`gcc -o yellow yellow.c canary32.c`

See `Dockerfile.yellow`

### spe

No building required.

### libyellow

Make sure to update the list of binary's you want to alert on in the alert_list array.

`gcc -shared -fPIC libyellow.c canary32.c -o libyellow.so`

See `Dockerfile.libyellow` or `Dockerfile.libyellow.debian` for Alpine and Debian versions.

### ldsoyellow

Check the `Dockerfile.ldsoyellow` for a working example. You'll need the glibc sources (I recommend using the official package version to be as close to the real linker as possible).

`apt-get source libc6`

Apply the patch `rtld.c.patch`

`cd /glibc-*`
`patch < rtld.c.patch`

Configure glibc with sanity checks disabled and pointing to the right libdir (assuming 64bit system)

`mkdir glibcbuild && cd glibcbuild`
`/glibc-*/configure --disable-sanity-checks --libdir=$(dirname $(find / -name "libc.so.6"|grep 64))`

Build as normal

`make`

## Installing

All variants use the TOKEN environment variable to specify the canary token URL. The environment variable name can be changed, or even hardcoded if you prefer. Again, Dockerfiles exist for each of these.

### yellow

yellow is intended to trigger on specific binaries you rename and alias to it, much like how busybox is used. You'll need to rename the binary to use the ".canary" extension, then create a symlink (or copy) of yellow for the original binary name. For example:

```
cp yellow /usr/bin
mv /usr/bin/id /usr/bin/id.canary
ln -s /usr/bin/yellow /usr/bin/id
```

After this any execution of `id` will trigger your canary token.

See `Dockerfile.yellow`

## spe

The sensitive process execution method uses the same technique as above, except it triggers with a shell script rather than a binary, and comes with an installer. Run `spe_install.sh` passing it your DNS token and name of the binary you want to trigger on. The binary doesn't need to exist.

For example:
`./spe_install.sh "c28y9l4dw0drj62un0cm4rwz6.canarytokens.com" /bin/id`

Will create an alert any time `id` is run.

This is a copy of the Windows token method described [here](https://blog.thinkst.com/2022/09/sensitive-command-token-so-much-offense.html)

### libyellow

libyellow is intended to trigger on any binary used where it has been injected via LD_PRELOAD but only alert on the ones you specify in the code. This can be done per session by setting the LD_PRELOAD environment variable to point to it, or across the system by adding it into the `/etc/ld.so.preload` file. The latter can break your system if the library isn't working on it, so it's recommended you test it using LD_PRELOAD first.

Remember to put your token in the TOKEN environment variable as described under "Pre-Work" above.

LD_PRELOAD example:
```
cp libyellow.so /usr/lib/
export LD_PRELOAD=/usr/lib/libyellow.so
```
Then run a target binary, check that it behaves as expected, and you get a canary alert.

Once you're happy everything is working, you can add it across the system with (this won't work on musl based systems like alpine, where the LD_PRELOAD method is used instead):
```
cp libyellow.so /usr/lib
echo /usr/lib/libyellow.so >> /etc/ld.so.preload
```

See `Dockerfile.libyellow` or `Dockerfile.libyellow.debian` for Alpine and Debian versions.

### ldsoyellow

ldsoyellow is intended to trigger on specific binaries where the binary is modified to use it instead of the default linker. You could replace the default linker with it for a system wide effect, but that would likely be too noisy.

The `Dockerfile.ldsoyellow` example includes doing this for `/bin/cat`. First the arm'ed linker is copied next to the legitemate linker:

`cp <arm'd linker> /lib64/ld-linux-x86-64.so.3`

Then the binary is modified to use it instead of the legitemate one:

`sed -i "s/\/lib64\/ld-linux-x86-64.so.2/\/lib64\/ld-linux-x86-64.so.3/" /bin/cat`

See `Dockerfile.ldsoyellow`.

## Docker in Docker Example

If you wanted to run Docker in Docker for, say, a build environment where you had limited control of the build directives, and be alerted if someone broke out, you could do something like the example in `Dockerfile.dind-rootless`. This example uses `yellow` but could use any or all the other variants.

It boobytraps binaries in the host which shouldn't be accessible from a container.
