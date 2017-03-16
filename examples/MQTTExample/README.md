## MQTT Example

### What it is

The example with the Erlang emqtt broker that is built in to the
[Zotonic](http://zotonic.com/) CMS. The site set up in Zotonic has to be dalen1
and you have to add a user "iot" so that the device can authenticate against it
using "iot@dalen1". The Zotonic site.erl and config is found in the erlsrc
directory. These files are to be in the user/sites/dalen1 directory under the
zotonic root. See also
[mod_mqtt](http://docs.zotonic.com/en/latest/ref/modules/mod_mqtt.html) in
Zotonic documentation.

It scans for WiFi networks and publishes them on topic site/sitename/iot/0001
where the sitename is set to dalen1 in the example. If the number of networks
is more than 10 the Zotonic implementation will publish a message on the
site/sitename/iotnotes on which the device is subscribing and if it gets
notified on this it will lit the LED for 10s.

The scanner, LED handler and MQTT client are set up in separate protothreads
and communicates using buffered datastreams.

The implementation depends on the library
[PubSubClient](https://github.com/knolleary/pubsubclient) by Nick O'Leary.

### Some notes

The emqtt broker used by Zotonic handles version 3.1 of the MQTT protocol.
Since the PubSubClient defaults to 3.1.1 one has set a define switch in
platformio.ini before building.
