# Using libschc pkg to compress udp over ipv6

## Cutie Notes:
- For libschc to compress a udp hdr it must be paired with an ipv6 hdr, this was mentione din the docs basically it's either ipv6 by itself or both udp and ipv6.

- An issue I encountered was that libschc would often skip the compression of the udp header, this was because i was passing the gnrc\_pktsnip\_t* of the ipv6 header which is FINE but it does not contain the udp header as payload and libschc does not follow the **next** pointer in the snip which points to the udp hdr, to fix this we call the gnrc\_pktbuf\_merge on the **ip** snip.

- For some reason (probably a skill issue from my part) i'm unable to index into the IP snips data, which as a reminder is a void\* to the actual packet header, you might try casting it to a uint8\_t* then indexing however this only allows you to access index 0 but doesn't seem to work for further indices (also i know this probably isnt safe anyways so GOOD). To index into a packet header and view its fields, cast the void\* to a ipv6\_hdr\_t\* and access that structs fields.  
