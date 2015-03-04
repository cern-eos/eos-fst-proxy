
eos-fst-proxy
=============

This project is a full copy of the XRootD project with the only modification done to the XrdPss subsystem. 
Instead of the usual **libXrdPss** library this project will build a library called **libXrdPssEos** which 
provides a custom frowarding proxy for the EOS subsystem. 

How it works
------------

The proxy inspects the opaque information for all requests and looks for the **eos.fstfrw** tag and then 
rewrites the URL using the value of this parameter as the endpoint to which it connects. This represents 
the actual FST endpoint to which the user is redirected by the MGM.

Configuration
-------------

The proxy is configured as a normal XRootD server and following is an example configuration file:

###########################################################
all.export / nolock
all.trace none
xrd.port 3001
xrootd.async off nosf
xrd.network keepalive
###########################################################
ofs.persist off
ofs.osslib /usr/local/lib64/libXrdPssEos-4.so
ofs.ckslib * /usr/local/lib64/libXrdPssEos-4.so
pss.origin =
ofs.trace all
###########################################################
xrootd.seclib libXrdSec.so
sec.protocol unix
sec.protbind * only unix
###########################################################

Therefore, the only library used from this project that is different from the standard XRootD is the libXrdPssEos.


