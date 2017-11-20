# Base info
C++ library based on [tproxy](https://www.kernel.org/doc/Documentation/networking/tproxy.txt) for modify network traffic route through your machine and examples how usage it.  **Before start run script iptables.sh as root.** By default redirect traffic from dst port 80 to port 9876. Edit it if needed.
### Examples (before usage make library)
* repeater - move data between client and server without modify traffic.
* http_replace - modify http not gziped data. Input parameters: old_word new_word. Test for it examples/http_replace/tests write with [CxxTest library](http://cxxtest.com/).

### API class Injector
| Name  | Description |
| ------------- | ------------- |
| **Injector (u_int16_t port)**  | Constructor for Injector class. Port for redirect iptables rule |
| **DEFAULT_PORT = 9876**  | Default port for redirect iptables rule  |
|  **void loop (injector_cb in_cb, injector_cb out_cb = DEF_CB)** | Method of class Injector. in_cb - callback for analysis input data from server, out_cb - callack for analysis input data from client (default is DEF_CB)   |
| **injector_cb** | typedef int (*injector_cb)(SockBridge &obj)|

### API class SockBridge 
| Name  | Description |
| ------------- | ------------- |
| **ssize_t read(u_int8_t \*arr, ssize_t br)**  | Read input data|
| **ssize_t write(u_int8_t \*arr, ssize_t br)**  | Read input data|
| **int get_from_fd() const**  | Get descriptor input descriptor|
| **int get_to_fd()**  | Get descriptor output descriptor|
