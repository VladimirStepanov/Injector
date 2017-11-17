# Base info
C++ library based on [tproxy](https://www.kernel.org/doc/Documentation/networking/tproxy.txt) for modify network traffic route through your machine and examples how usage it.  **Before start run script iptables.sh as root.** By default redirect traffic from dst port 80 to port 9876. Edit it if needed.
# Examples (before usage make library)
* repeater - move data between client and server without modify traffic.
* http_replace - modify http not gziped data. Input parameters: old_word new_word. Test for it examples/http_replace/tests write with [CxxTest library](http://cxxtest.com/).
